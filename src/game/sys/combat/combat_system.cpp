#include "combat_system.hpp"

#include "../state/state_comp.hpp"
#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

#include <core/asset/aid.hpp>
#include <core/utils/random.hpp>
#include <core/utils/template_utils.hpp>

#include "comp/friend_comp.hpp"
#include "comp/score_comp.hpp"

#include "comp/bullet_comp.hpp"


namespace mo {
namespace sys {
namespace combat {

	using namespace state;
	using namespace unit_literals;

	namespace {
		auto rng = util::create_random_generator();

		Angle random_angle(Angle min_max) {
			return Angle{util::random_real(rng, remove_unit(-min_max), remove_unit(min_max))};
		}
	}

	Combat_system::Combat_system(asset::Asset_manager& assets,
	                             ecs::Entity_manager& entity_manager,
	                             physics::Transform_system& ts,
	                             physics::Physics_system& physics_system,
	                             state::State_system& state_system,
	                             Effect_source& effects)
	    : _em(entity_manager),
	      _weapons(entity_manager.list<Weapon_comp>()),
	      _healths(entity_manager.list<Health_comp>()),
	      _explosives(entity_manager.list<Explosive_comp>()),
	      _lsights(entity_manager.list<Laser_sight_comp>()),
	      _ts(ts),
	      _collision_slot(&Combat_system::_on_collision, this),
	      _reaper(entity_manager, state_system),
	      _ray_renderer(assets),
	      _effects(effects)
	{

		_collision_slot.connect(physics_system.collisions);

		_em.register_component_type<Friend_comp>();
		_em.register_component_type<Weapon_comp>();
		_em.register_component_type<Health_comp>();
		_em.register_component_type<Explosive_comp>();
		_em.register_component_type<Bullet_comp>();
		_em.register_component_type<Score_comp>();
	}

	void Combat_system::update(Time dt) {
		_shoot_something(dt);
		_explode_explosives(dt);
		_health_care(dt);
	}
	void Combat_system::draw(const renderer::Camera& cam) {
		_ray_renderer.set_vp(cam.vp());

		for(auto& l : _lsights) {
			_draw_ray(l);
		}
	}

	void Combat_system::_health_care(Time dt) {
		for(auto& h : _healths) {
			auto auto_heal = 0.f;
			if(h.damaged() && h._damage==0)
				auto_heal = h._auto_heal * dt.value();

			auto health_mod = h._heal+auto_heal - h._damage;

			h._current_hp = std::min(h._current_hp+health_mod, h._max_hp);

			if(h._damage > h._heal+auto_heal) {
				h.owner().get<State_comp>().process([](auto& s){
					s.state(Entity_state::damaged);
				});
			} else if(h._damage < h._heal+auto_heal) {
				h.owner().get<State_comp>().process([](auto& s){
					s.state(Entity_state::healed);
				});
			}

			h._damage = 0;
			h._heal = 0;

			if(h._current_hp<=0) {
				auto explosive = h.owner().get<Explosive_comp>();
				auto explode = explosive.process(false, [](const auto& e){return e._activate_on_damage && !e._exloded;});

				if(explode) {
					explosive.process([&](auto& e){
						if(e._delay_left<=0_s)
							e._delay_left = max(e._delay, 1_ms);
					});

				} else {
					h.owner().get<State_comp>().process([](auto& s){
						s.state(Entity_state::dying);
					});
				}
			}
		}
	}

	void Combat_system::_shoot_something(Time dt) {
		for(auto& w : _weapons) {
			auto weapon = w._weapon;

			for(auto& m : _modifiers) {
				m->process(w.owner(), weapon);
			}

			if(w._cooldown_left>0_s) {
				w._cooldown_left = w._cooldown_left-dt;
				if(w._cooldown_left<0_s)
					w._cooldown_left=0_s;

				w._attack = false;
				continue;
			}

			bool attack_now = false;

			if(w._attack) {
				_effects.inform(w.owner(), weapon.effect);
				w.owner().get<State_comp>().process([&](auto& s){
					if(weapon.type==combat::Weapon_type::melee)
						s.state(Entity_state::attacking_melee);
					else
						s.state(Entity_state::attacking_range);
				});

				if(weapon.attack_delay==0_s)
					attack_now = true;
				else if(w._attack_delay_left<=0_s)
					w._attack_delay_left = weapon.attack_delay;

				w._attack = false;
			}

			if(w._attack_delay_left>0_s) {
				w._attack_delay_left -= dt;
				if(w._attack_delay_left<=0_s) {
					w._attack_delay_left = 0_s;
					attack_now = true;
				}
			}

			if(attack_now) {
				on_attack(w.owner(), weapon);

				auto& transform = w.owner().get<physics::Transform_comp>().get_or_throw();
				auto position = transform.position();
				auto rotation = transform.rotation();

				auto& physics = w.owner().get<physics::Physics_comp>().get_or_throw();
				auto radius = physics.radius();

				auto group = w.owner().get<Friend_comp>().process(0, [](const auto& f){return f.group();});

				switch(weapon.type) {
					case Weapon_type::melee:
						_ts.foreach_in_range(position, rotation, weapon.melee_range,
						                     weapon.melee_range, weapon.melee_angle,
						                     weapon.melee_angle,
						                     [&](ecs::Entity& t) {
							if(&t!=&w.owner()) {
								_deal_damage(t, group, weapon.melee_damage);
							}
						});
						break;

					case Weapon_type::range:
						if(weapon.bullet_type) {
							physics.impulse(rotate(glm::vec2(-1,0), rotation) * weapon.recoil);

							for(int c=0; c<weapon.bullet_count; ++c) {
								auto bullet = _em.emplace(weapon.bullet_type);
								bullet->get<Friend_comp>().process([&](auto& f) {
									f.group(group);
								});

								auto bullet_rot = rotation + random_angle(weapon.spreading);

								auto& bullet_phys = bullet->get<physics::Physics_comp>().get_or_throw();
								auto bullet_radius = bullet_phys.radius();

								bullet_phys.velocity(physics.velocity() + rotate(Velocity{weapon.bullet_vel,0}, bullet_rot));

								bullet->get<physics::Transform_comp>().process([&](auto& t){
									t.position(position + rotate(Position{radius+bullet_radius+10_cm, 0_m}, bullet_rot));
									t.rotation(bullet_rot);
								});
							}
						}
						break;
				}

				w._cooldown_left = weapon.cooldown;
			}
		}
	}

	void Combat_system::_explode_explosives(Time dt) {
		for(auto& e : _explosives) {
			if(e._delay_left>0_s) {
				e._delay_left-=dt;

				if(e._delay_left<=0_s) {
					_explode(e);
				}
			}
		}
	}

	void Combat_system::_explode(Explosive_comp& e) {
		if(e._exloded)
			return;
		e._exloded = true;

		auto& transform = e.owner().get<physics::Transform_comp>().get_or_throw();
		auto position = transform.position();
		auto rotation = transform.rotation();
		auto group = e.owner().get<Friend_comp>().process(0, [](const auto& f){return f.group();});

		_ts.foreach_in_range(position, rotation, e._range, e._range, 360_deg, 360_deg,
							 [&](ecs::Entity& t){
			if(&t!=&e.owner()) {
				this->_deal_damage(t, group, e._damage, e._damage_type);
				t.get<physics::Physics_comp>().process([&](auto& p){
					auto dir = remove_units(t.get<physics::Transform_comp>().get_or_throw().position()-position);
					glm::normalize(dir);
					p.impulse(dir * e._blast_force);
				});
			}
		});

		e.owner().get<State_comp>().process([](auto& s) {
			s.state(Entity_state::dying);
		});

		_effects.inform(e.owner(), e._explosion_effect);
	}

	bool Combat_system::_deal_damage(ecs::Entity& target, int group, float damage,
	                                 level::Element type) {
		bool dealed = false;

		target.get<Health_comp>().process([&](Health_comp& h){
			if(group!=0) {
				auto tgroup = target.get<Friend_comp>().process(0,
								[](const auto& f){return f.group();});
				if(group==tgroup)
					return;
			}

			h.damage(damage, type);
			dealed = true;
		});

		return dealed;
	}

	void Combat_system::_draw_ray(Laser_sight_comp& l) {
		l.owner().get<sys::physics::Transform_comp>().process(
			[&](sys::physics::Transform_comp& t) {
				Distance dist = 20_m;
				util::maybe<ecs::Entity&> entity = util::nothing();

				std::tie(entity, dist) =
						_ts.raycast_nearest_entity(t.position(),
														 t.rotation(),
														 20_m,
														 [&](ecs::Entity& e){
					return &l.owner()!=&e && e.get<sys::physics::Transform_comp>().get_or_throw().layer()>=0.5;
				});

				auto p = remove_units(t.position());

				entity.process([&](ecs::Entity& e){
					dist = std::max(dist,Distance(glm::length(p - remove_units(e.get<sys::physics::Transform_comp>().get_or_throw().position()))));
				});

				_ray_renderer.draw(glm::vec3(p.x,p.y,0.49), t.rotation(), dist.value(), l.color(), l.width());
		});
	}

	void Combat_system::_on_collision(physics::Manifold& m) {
		m.a->owner().get<Explosive_comp>().process([&](auto& e) {
			if(m.is_with_object() &&
			   m.b.comp->owner().get<physics::Transform_comp>().get_or_throw().layer()<0.5) {
				return;
			}

			if(e._activate_on_contact) {
				if(e._delay>0_s) {
					if(e._delay_left<=0_s)
						e._delay_left = e._delay;

				} else
					this->_explode(e);
			}
		});


		m.a->owner().get<Bullet_comp>().process([&](Bullet_comp& b) {
			bool broken = false;

			if(m.is_with_object()) {

				auto group = b.owner().get<Friend_comp>().process(0, [](const auto& f){return f.group();});

				this->_deal_damage(m.b.comp->owner(), group, b._damage, b._damage_type);

				if(m.b.comp->owner().get<physics::Transform_comp>().get_or_throw().layer()>=0.5) {
					broken = --b._break_after_entities <=0;

					_effects.inform(b.owner(), b._effect);
				}

			}else {
				broken = --b._break_after_walls <=0;

				_effects.inform(b.owner(), b._effect);
			}

			if(broken) {
				b.owner().get<State_comp>().process([](auto& s){
					s.state(Entity_state::dying);

				}).on_nothing([&](){
					b.owner().erase<Bullet_comp>();
				});
			}
		});
	}

}
}
}

