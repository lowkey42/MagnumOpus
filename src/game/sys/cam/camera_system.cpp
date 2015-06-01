#include "camera_system.hpp"

#include <game/game_engine.hpp>

#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

namespace mo {
namespace sys {
namespace cam {

	using namespace unit_literals;

	constexpr auto world_scale = 16.f*3; // pixel/meter
	constexpr auto vscreen_height = 512;

	VScreen::VScreen(glm::vec2 size, float world_scale)
		: camera(size, world_scale), vscreen(size.x, size.y, true) {
	}

	Camera_system::Camera_system(ecs::Entity_manager& entity_manager, Game_engine& engine)
		: _gctx(engine.graphics_ctx()),
	      _targets(entity_manager.list<Camera_target_comp>()),
		  _vscreen_size(renderer::calculate_vscreen(engine, vscreen_height)),
	      _main_camera({engine.graphics_ctx().win_width(), engine.graphics_ctx().win_height()}, world_scale) {

		entity_manager.register_component_type<Camera_target_comp>();

		_cameras.emplace_back(_vscreen_size, world_scale);
		_cameras.back().camera.zoom(0.75f);
	}

	void Camera_system::update(Time dt) {
		for(auto& target : _targets) {

			if(target._rotation_zoom_time_left>0_s)
				target._rotation_zoom_time_left-=dt;

			Position target_offset{0_m, 0_m};

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

				target._last_rotation = target._last_rotation*(1-dt.value()*4.f) + target_rot*dt.value()*4.f;

				target_offset += rotate(Position{3_m, 0_m}, target._last_rotation);

				target.chase(target_pos, dt);
			});

			// TODO: add support for multiple cameras
			_cameras.back().camera.position(remove_units(target.cam_position() + target_offset));
			_cameras.back().targets.clear();
			_cameras.back().targets.push_back(target.owner_ptr());
		}

		_main_camera.position(_cameras.front().camera.position());
		_main_camera.zoom(_cameras.front().camera.zoom());
	}

}
}
}
