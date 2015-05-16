#include "ai_system.hpp"

#include "target_tag_comp.hpp"
#include "../physics/transform_system.hpp"

#include "../../level/level.hpp"

#include <game/game_engine.hpp>

#include <core/units.hpp>

namespace mo {
namespace sys {
namespace ai {

	using namespace unit_literals;

	Ai_system::Ai_system(ecs::Entity_manager& entity_manager, Game_engine& engine,
	                     physics::Transform_system& transform_system, level::Level& level)
	    : _engine(engine), _simples(entity_manager.list<Simple_ai_comp>()),
	      _transform_system(transform_system), _level(level) {

		entity_manager.register_component_type<Simple_ai_comp>();
		entity_manager.register_component_type<Target_tag_comp>();
	}

	void Ai_system::update(Time dt) {
		_engine.assets();
		for(auto& e : _simples) {
			e.owner().get<physics::Transform_comp>().process([&](auto& trans){
				ecs::Entity* target_entity = nullptr;

				auto flocking = e._wander_dir;
				auto flocking_count = 1.f;


				// TODO: keep distance; use target_pos instead of direction

				_transform_system.foreach_in_range(trans.position(), trans.rotation(), e.near, e.max, e.far_angle, e.near_angle,
						[&](ecs::Entity& target){

					if(target.has<Target_tag_comp>())
						target_entity = &target;

					target.get<Simple_ai_comp>().process([&](auto& ai){
						if(e._swarm_id>=0 && e._swarm_id==ai._swarm_id) {
							flocking+=ai._wander_dir;
							flocking_count++;
							constexpr auto target_weight = 5.f;
							if(ai._target) {
								flocking+=ai._wander_dir * target_weight;
								flocking_count+=target_weight;
							}
						}
					});
				});

				e._wander_dir = e._wander_dir*0.5f + (flocking/flocking_count)*0.5f;

				if(target_entity) {
					e.target(target_entity->shared_from_this());

				}else{
					e.no_target(dt, _level);
				}
			});
		}
	}

}
}
}
