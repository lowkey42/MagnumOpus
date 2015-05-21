#include "camera_system.hpp"

#include <game/game_engine.hpp>

#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

namespace mo {
namespace sys {
namespace cam {

	using namespace unit_literals;

	constexpr auto world_scale = 48.f; // pixel/meter
	constexpr auto vscreen_height = 720;

	VScreen::VScreen(glm::vec2 size, float world_scale)
		: camera(size, world_scale), vscreen(size.x, size.y, true) {
	}

	Camera_system::Camera_system(ecs::Entity_manager& entity_manager, Game_engine& engine)
		: _engine(engine),
		  _targets(entity_manager.list<Camera_target_comp>()),
		  _vscreen_size(renderer::calculate_vscreen(engine, vscreen_height)) {

		entity_manager.register_component_type<Camera_target_comp>();

		_cameras.emplace_back(_vscreen_size, 48.f);
	}

	void Camera_system::update(Time dt) {
		for(auto& target : _targets) {

			if(target._rotation_zoom_time_left>0_s)
				target._rotation_zoom_time_left-=dt;

			target.owner().get<physics::Transform_comp>().process([&](auto& transform) {
				auto target_pos = transform.position();
				auto target_rot = transform.rotation();
				auto moving = false;

				// TODO[foe]: refactor
				target.owner().get<physics::Physics_comp>().process([&](auto& p){
					target_pos += p.velocity()*100_ms;

					auto tpnt = remove_units(p.velocity()*100_ms);
					moving = tpnt.x*tpnt.x + tpnt.y*tpnt.y >= 0.1f;
				});

				if(std::abs(target._last_rotation-target_rot)>10_deg || moving) {
					target._rotation_zoom_time_left = target._rotation_zoom_time;
					target._last_rotation=target_rot;
				}

/*
				float p = target._rotation_zoom_time_left / target._rotation_zoom_time;

				target_pos += rotate(Position{3_m*p, 0_m}, transform.rotation());
				*/

				target.chase(target_pos, dt);
			});

			// TODO: add support for multiple cameras
			_cameras.back().camera.position(remove_units(target.cam_position()));
			_cameras.back().targets.clear();
			_cameras.back().targets.push_back(target.owner_ptr());
		}
	}

}
}
}
