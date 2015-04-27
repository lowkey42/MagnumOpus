#include "camera_system.hpp"

#include <game/game_engine.hpp>

#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

namespace mo {
namespace sys {
namespace cam {

	using namespace unit_literals;

	Camera_system::Camera_system(ecs::Entity_manager& entity_manager, Game_engine& engine)
		: _engine(engine), _targets(entity_manager.list<Camera_target_comp>()) {
		entity_manager.register_component_type<Camera_target_comp>();

		_cameras.emplace_back(_engine, 32.f);
	}

	void Camera_system::update(Time dt) {
		for(auto& target : _targets) {

			target.owner().get<physics::Transform_comp>().process([&](auto& transform) {
				auto target_pos = transform.position();

				target.owner().get<physics::Physics_comp>().process([&](auto& p){
					target_pos= target_pos + p.velocity()*500_ms;
				});

				target.chase(target_pos, dt);
			});

			// TODO: add support for multiple cameras
			_cameras.back().position(remove_units(target.cam_position()));
		}
	}

}
}
}
