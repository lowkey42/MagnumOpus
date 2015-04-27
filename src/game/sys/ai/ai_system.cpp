#include "ai_system.hpp"

#include "target_tag_comp.hpp"
#include "../physics/transform_system.hpp"

#include <game/game_engine.hpp>

#include <core/units.hpp>

namespace mo {
namespace sys {
namespace ai {

	using namespace unit_literals;

	Ai_system::Ai_system(ecs::Entity_manager& entity_manager, Game_engine& engine, physics::Transform_system& transform_system)
	    : _engine(engine), _simples(entity_manager.list<Simple_ai_comp>()), _transform_system(transform_system) {

		entity_manager.register_component_type<Simple_ai_comp>();
		entity_manager.register_component_type<Target_tag_comp>();
	}

	void Ai_system::update(Time dt) {
		_engine.assets();
		for(auto& e : _simples) {
			e.owner().get<physics::Transform_comp>().process([&](auto& trans){
				ecs::Entity* target_entity = nullptr;

				_transform_system.foreach_in_range(trans.position(), trans.rotation(), e.near, e.max, e.far_angle, e.near_angle,
						[&](ecs::Entity& target){

					if(target.has<Target_tag_comp>())
						target_entity = &target;
				});

				if(target_entity) {
					e.target(target_entity->shared_from_this());

				}else{
					e.no_target(dt);
				}
			});
		}
	}

}
}
}
