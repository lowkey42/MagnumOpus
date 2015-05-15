/**************************************************************************\
 * camera.hpp - Defining a movable and scalable camera w viewport         *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <glm/glm.hpp>

#include "../engine.hpp"

namespace mo {
namespace renderer {

	class Camera {

	public:

		// Constructors
		Camera(const Engine &engine, float world_scale=1.f,
		       const glm::vec2 position=glm::vec2(0.0f), float zoom=1.0f) noexcept;

		// Methods
		void position(glm::vec2 pos) noexcept {_pos = pos; _dirty = true;}
		void move(glm::vec2 offset) noexcept {_pos+=offset; _dirty = true;}
		void zoom(float z) noexcept {_zoom=z; _dirty=true;}
		void viewport(glm::vec4 viewport)noexcept;

		auto zoom() const noexcept { return _zoom; }
		auto position() const noexcept { return _pos; }
		auto viewport() const noexcept { return _viewport; }
        auto world_scale() const noexcept { return _world_scale; }

		auto vp() const noexcept -> const glm::mat4&;

		auto screen_to_world(const glm::vec2 screen_pos) const noexcept -> glm::vec2;
		auto world_to_screen(const glm::vec2 world_pos) const noexcept -> glm::vec2;

	private:
		void recalc_vp()const noexcept;

		glm::vec4 _viewport; //< x,y,w,h
		glm::mat4 _projection;

		const float _world_scale;
		float _zoom;
		glm::vec2 _pos;
		mutable glm::mat4 _vp;
		mutable bool _dirty;

	};
}
}
