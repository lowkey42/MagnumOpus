#define GLM_SWIZZLE

#include "camera.hpp"

#include "graphics_ctx.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <SDL2/SDL.h>
#include <GL/glew.h>


namespace mo {
namespace renderer {

	glm::vec2 calculate_vscreen(const Engine &engine, int target_height) {
		float width = engine.graphics_ctx().win_width();
		float height = engine.graphics_ctx().win_height();

		float vheight = height/std::round(height/target_height);
		float vwidth  = width/height * vheight;

	//	INFO("virtual resolution: "<<width<<"X"<<height);

		return {vwidth, vheight};
	}

	namespace {
		auto calc_viewport(int width, int height) -> glm::vec4 {
			return {0,0, width, height};
		}
		auto calc_projection(const glm::vec4& viewport) -> glm::mat4 {
			auto height = viewport[3];
			auto width  = viewport[2];

			return glm::ortho(-width/2.f, width/2.f,
				              height/2.f, -height/2.f, -1.0f, 1.0f);
		}
	}

	// Constructors
	Camera::Camera(glm::vec2 size, float world_scale,
	               const glm::vec2 position, float zoom) noexcept
		: _viewport(calc_viewport(size.x, size.y)),
	      _projection(calc_projection(_viewport)),
	      _world_scale(world_scale),
	      _zoom(zoom),
	      _pos(position),
	      _dirty(true) {
	}


	// Methods
	void Camera::viewport(glm::vec4 viewport)noexcept {
		_viewport = viewport;
		_projection = calc_projection(_viewport);
		_dirty = true;
	}
	void Camera::recalc_vp()const noexcept {
		auto z = _zoom*_world_scale;
		auto scale = glm::scale(glm::mat4(1.0f),
								glm::vec3(z, z, 1.f));
		auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(std::round(-_pos.x*z)/z, std::round(-_pos.y*z)/z, 0));
		_vp = _projection * scale * trans;
		_dirty = false;
	}

	const glm::mat4& Camera::vp() const noexcept {
		if(_dirty){
			recalc_vp();
		}
		return _vp;
	}

	glm::vec2 Camera::screen_to_world(glm::vec2 screen_pos) const noexcept {
		screen_pos.y = _viewport[3]-screen_pos.y;

		return glm::unProject(glm::vec3(screen_pos.xy(),0), glm::mat4(1), vp(), _viewport).xy();
	}

	glm::vec2 Camera::world_to_screen(const glm::vec2 world_pos) const noexcept {
		auto r = glm::project(glm::vec3(world_pos.xy(),0), glm::mat4(1), vp(), _viewport);

		return {r.x, _viewport[3]-r.y};
	}

	void Camera::bind_viewport()const noexcept {
		glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
	}
}
}
