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
	constexpr auto min_zoom = 0.5f;
	constexpr auto max_zoom = 0.75f;

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
		_cameras.back().camera.zoom(max_zoom);
	}

	void Camera_system::update(Time dt) {
		glm::vec2 pos_acc, pos_min{99999.f, 99999.f}, pos_max;
		float pos_count=0;

		_cameras.back().targets.clear();

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

			_cameras.back().targets.push_back(target.owner_ptr());
			auto p = remove_units(target.cam_position() + target_offset);
			pos_acc+=p;
			pos_count++;

			if(pos_min.x>p.x) pos_min.x = p.x;
			if(pos_min.y>p.y) pos_min.y = p.y;
			if(pos_max.x<p.x) pos_max.x = p.x;
			if(pos_max.y<p.y) pos_max.y = p.y;
		}

		if(pos_count>0) {
			auto& cam = _cameras.back().camera;
			auto cam_pos = pos_acc / pos_count;
			cam.position(cam_pos*0.25f + cam.position()*0.75f);
			float max_dist = glm::length(pos_max-pos_min) + 1;

			auto new_zoom = glm::clamp((vscreen_height/world_scale)/max_dist, min_zoom, max_zoom);
			cam.zoom(cam.zoom()*0.8f + new_zoom*0.2f);
		}

		_main_camera.position(_cameras.front().camera.position());
		_main_camera.zoom(_cameras.front().camera.zoom());
	}

}
}
}
