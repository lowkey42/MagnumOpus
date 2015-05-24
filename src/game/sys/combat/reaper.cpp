#include "reaper.hpp"

#include <core/ecs/ecs.hpp>
#include <core/units.hpp>
#include <core/utils/random.hpp>

#include "../sprite/sprite_comp.hpp"
#include "../state/state_comp.hpp"
#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"
#include "score_comp.hpp"
#include "explosive_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace state;
	using namespace unit_literals;
	using namespace state;

	namespace {
		auto rng = util::create_random_generator();

		glm::vec2 random_dir() {
			return rotate(glm::vec2{1,0}, Angle{util::random_real(rng, remove_unit(0_deg), remove_unit(360_deg))});
		}
	}

	Reaper::Reaper(ecs::Entity_manager& entity_manager,
				   state::State_system& state_system)
		: _em(entity_manager), _reap_slot(&Reaper::_reap, this) {

		_reap_slot.connect(state_system.state_change_events);
	}

	void Reaper::_reap(ecs::Entity& e, sys::state::State_data& s) {
		if(s.s==Entity_state::dying) {
			// he's dead jim

			e.get<physics::Transform_comp>().process([&](auto& transform){
				transform.layer(0.1f);

				e.get<Score_comp>().process([&](auto& s){
					s._collectable = true;
					s._collected = true;
					for(int i=0; i<s._value; ++i) {
						auto coin = _em.emplace("blueprint:coin"_aid);
						coin->get<physics::Transform_comp>().get_or_throw().position(transform.position());
						coin->get<physics::Physics_comp>().get_or_throw().impulse(random_dir()  *20_N);
					}
				});
			});

			auto explosive = e.get<Explosive_comp>();
			auto explode = explosive.process(false, [](const auto& e){return e._activate_on_damage && !e._exloded;});

			if(!explode) {
				if(e.get<State_comp>().get_or_throw().delete_dead())
					_em.erase(e.shared_from_this());

				else // remove components:
					e.erase_other<
							Explosive_comp,
							physics::Transform_comp,
							sprite::Sprite_comp,
							state::State_comp>();

			} else {
				explosive.process([&](auto& e){
					if(e._delay_left<=0_s)
						e._delay_left = max(e._delay, 1_ms);
				});
			}
		}
	}


}
}
}

