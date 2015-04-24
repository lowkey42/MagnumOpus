#include "camera_system.hpp"

#include "../../../game_engine.hpp"

#include "../physics/transform_comp.hpp"

namespace game {
namespace sys {
namespace cam {

	Camera_system::Camera_system(core::ecs::Entity_manager& entity_manager, Game_engine& engine)
		: _engine(engine), _targets(entity_manager.list<Camera_target_comp>()) {
		entity_manager.register_component_type<Camera_target_comp>();

		_cameras.emplace_back(_engine, 16.f);
	}

	void Camera_system::update(core::Time dt) {
		for(auto& target : _targets) {

			target.owner().get<physics::Transform_comp>().process([&](auto& transform){
				target.chase(transform.position(), dt);
			});

			// TODO: add support for multiple cameras
			_cameras.back().position(core::remove_units(target.cam_position()));
		}
	}

}
}
}
