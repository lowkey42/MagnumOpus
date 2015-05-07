#include "combat_system.hpp"

#include "../state/state_comp.hpp"
#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

#include <core/asset/asset_manager.hpp> // TODO[foe]: remove after sprite_comp integration
#include "../controller/controllable_comp.hpp" // TODO[foe]: remove
#include <core/asset/aid.hpp>
#include "../sprite/sprite_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace state;
	using namespace unit_literals;

	Combat_system::Combat_system(ecs::Entity_manager& entity_manager,
								 asset::Asset_manager& assets,
	                             physics::Transform_system& ts)
		: _em(entity_manager), _assets(assets),
		  _weapons(entity_manager.list<Weapon_comp>()),
		  _healths(entity_manager.list<Health_comp>()),
		  _ts(ts) {

	}

	void Combat_system::update(Time dt) {
		_shoot_something(dt);
		_health_care(dt);
	}

	void Combat_system::_health_care(Time dt) {
		for(auto& h : _healths) {
			auto auto_heal = 0.f;
			if(h.damaged())
				auto_heal = h._auto_heal * dt.value();

			auto health_mod = h._heal+auto_heal - h._damage;
			if(health_mod<-h._max_hp/10.f)
				health_mod = -h._max_hp/10.f;

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
					s.state(Entity_state::died);
				});
				_em.erase(h.owner_ptr());
			}
		}
	}

	void Combat_system::_shoot_something(Time dt) {
		for(auto& w : _weapons) {
			if(w._attack) {
				w.owner().get<State_comp>().process([&w](auto& s){
					if(w.weapon_type()==combat::Weapon_type::melee)
						s.state(Entity_state::attacking_melee);
					else
						s.state(Entity_state::attacking_range);
				});
			}

			if(w._cooldown_left>0_s) {
				w._cooldown_left = w._cooldown_left-dt;
				if(w._cooldown_left<0_s)
					w._cooldown_left=0_s;

				w._attack = false;
				continue;
			}

			if(w._attack) {
				auto& transform = w.owner().get<physics::Transform_comp>().get_or_throw();
				auto position = transform.position();
				auto rotation = transform.rotation();

				auto& physics = w.owner().get<physics::Physics_comp>().get_or_throw();
				auto radius = physics.radius();

				switch(w._type) {
					case Weapon_type::melee:
						DEBUG("melee attack"<<(void*)&w.owner());
						_ts.foreach_in_range(position, rotation, 1_m, 1.5_m, 90_deg, 100_deg,
						                     [&](ecs::Entity& t){
							t.get<Health_comp>().process([&](Health_comp& h){
								h.damage(10);
								DEBUG("  damaged "<<(void*)&h.owner());
							});
						});
						// TODO
						break;

					case Weapon_type::range:
						if(w._bullet_type) {
							auto bullet = _em.emplace(w._bullet_type);
							auto& bullet_phys = bullet->get<physics::Physics_comp>().get_or_throw();
							auto bullet_radius = bullet_phys.radius();

							bullet_phys.velocity(physics.velocity() + rotate(Velocity{w._bullet_vel,0}, rotation));

							bullet->get<physics::Transform_comp>().process([&](auto& t){
								t.position(position + rotate(Position{radius+bullet_radius+10_cm, 0_m}, rotation));
								t.rotation(rotation);
							});

							// TODO[foe]: remove after sprite_comp integration

							auto anim = _assets.load<renderer::Animation>("anim:ball"_aid);
							bullet->emplace<sys::sprite::Sprite_comp>(anim);
							// END TODO
						}
						break;
				}

				w._cooldown_left = w._cooldown;
				w._attack = false;
			}
		}
	}

}
}
}

