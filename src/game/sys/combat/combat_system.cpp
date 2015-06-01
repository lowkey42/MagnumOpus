#include "combat_system.hpp"

#include "../state/state_comp.hpp"
#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

#include <core/asset/aid.hpp>

#include "friend_comp.hpp"
#include "score_comp.hpp"
#include "collector_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace state;
	using namespace unit_literals;

	Combat_system::Combat_system(ecs::Entity_manager& entity_manager,
								 physics::Transform_system& ts,
								 physics::Physics_system& physics_system,
								 state::State_system& state_system)
		: _em(entity_manager),
		  _weapons(entity_manager.list<Weapon_comp>()),
		  _healths(entity_manager.list<Health_comp>()),
		  _explosives(entity_manager.list<Explosive_comp>()),
		  _ts(ts),
		  _collision_slot(&Combat_system::_on_collision, this),
		  _reaper(entity_manager, state_system)
	{

		_collision_slot.connect(physics_system.collisions);

		_em.register_component_type<Friend_comp>();
		_em.register_component_type<Weapon_comp>();
		_em.register_component_type<Health_comp>();
		_em.register_component_type<Explosive_comp>();
		_em.register_component_type<Score_comp>();
		_em.register_component_type<Collector_comp>();
	}

	void Combat_system::update(Time dt) {
		_shoot_something(dt);
		_explode_explosives(dt);
		_health_care(dt);
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
				h.owner().get<State_comp>().process([](auto& s){
					s.state(Entity_state::dying);
				});
			}
		}
	}

	void Combat_system::_shoot_something(Time dt) {
		for(auto& w : _weapons) {
			if(w._cooldown_left>0_s) {
				w._cooldown_left = w._cooldown_left-dt;
				if(w._cooldown_left<0_s)
					w._cooldown_left=0_s;

				w._attack = false;
				continue;
			}

			bool attack_now = false;

			if(w._attack) {
				w.owner().get<State_comp>().process([&w](auto& s){
					if(w.weapon_type()==combat::Weapon_type::melee)
						s.state(Entity_state::attacking_melee);
					else
						s.state(Entity_state::attacking_range);
				});

				if(w._attack_delay==0_s)
					attack_now = true;
				else if(w._attack_delay_left<=0_s)
					w._attack_delay_left = w._attack_delay;

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
				auto& transform = w.owner().get<physics::Transform_comp>().get_or_throw();
				auto position = transform.position();
				auto rotation = transform.rotation();

				auto& physics = w.owner().get<physics::Physics_comp>().get_or_throw();
				auto radius = physics.radius();

				auto group = w.owner().get<Friend_comp>().process(0, [](const auto& f){return f.group();});

				switch(w._type) {
					case Weapon_type::melee:
						_ts.foreach_in_range(position, rotation, w._melee_range,
											 w._melee_range, w._melee_angle, w._melee_angle,
						                     [&](ecs::Entity& t){
							if(&t!=&w.owner()) {
								_deal_damage(t, group, w._melee_damage);
							}
						});
						break;

					case Weapon_type::range:
						if(w._bullet_type) {
							physics.impulse(rotate(glm::vec2(-1,0), rotation) * w._recoil);
							auto bullet = _em.emplace(w._bullet_type);
							bullet->get<Friend_comp>().process([&](auto& f) {
								f.group(group);
							});

							auto& bullet_phys = bullet->get<physics::Physics_comp>().get_or_throw();
							auto bullet_radius = bullet_phys.radius();

							bullet_phys.velocity(physics.velocity() + rotate(Velocity{w._bullet_vel,0}, rotation));

							bullet->get<physics::Transform_comp>().process([&](auto& t){
								t.position(position + rotate(Position{radius+bullet_radius+10_cm, 0_m}, rotation));
								t.rotation(rotation);
							});
						}
						break;
				}

				w._cooldown_left = w._cooldown;
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
				this->_deal_damage(t, group, e._damage);
				t.get<physics::Physics_comp>().process([&](auto& p){
					auto dir = remove_units(t.get<physics::Transform_comp>().get_or_throw().position()-position);
					glm::normalize(dir);
					p.impulse(dir * e._blast_force);
				});
			}
		});

		//_em.erase(e.owner_ptr());
		e.owner().get<State_comp>().process([](auto& s){
			s.state(Entity_state::dying);
		});
	}

	void Combat_system::_deal_damage(ecs::Entity& target, int group, float damage) {
		target.get<Health_comp>().process([&](Health_comp& h){
			if(group!=0) {
				auto tgroup = target.get<Friend_comp>().process(0,
								[](const auto& f){return f.group();});
				if(group==tgroup)
					return;
			}

			h.damage(damage);
		});
	}

	void Combat_system::_on_collision(physics::Manifold& m) {
		if(m.is_with_object()) {
			m.a->owner().get<Score_comp>().process([&](auto& s){
				if(s._collectable && !s._collected) {
					m.b.comp->owner().get<Score_comp>().process([&](auto& os) {
						if(os._collector) {
							os._value+=s._value;
							s._collected = true;
							_em.erase(s.owner_ptr());
						}
					});
				}
			});
		}

		m.a->owner().get<Explosive_comp>().process([&](auto& e) {
			if(e._activate_on_contact) {
				if(e._delay>0_s) {
					if(e._delay_left<=0_s)
						e._delay_left = e._delay;

				} else
					this->_explode(e);
			}
		});
	}

}
}
}

