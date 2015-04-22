/**************************************************************************\
 * sprite_batch.hpp - drawing sprites on screen                           *
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

#include "vertex_object.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "texture.hpp"

#include "../../core/units.hpp"

#include <vector>


namespace core {
namespace renderer {

	class Sprite_batch {

	public:

		struct TileVertex {
			glm::vec2 pos;
			glm::vec2 uv;
		};

		struct Sprite{
			core::Position position;
			float rotation;
			core::asset::AID texture;
			glm::vec4 uv;
		};

		// Constructors
		Sprite_batch(core::asset::Asset_manager& asset_manager);

		// Methods
		void draw(const core::renderer::Camera& cam, Sprite& sprite) const noexcept;
		void drawAll(const core::renderer::Camera& cam) const noexcept;


	private:

		core::asset::Asset_manager& _asset_manager;

		mutable core::renderer::Object _object;
		mutable core::renderer::Shader_program _shader;
		mutable core::asset::Ptr<core::renderer::Texture> _texture;

		mutable std::vector<TileVertex> _vertices;


	};

}
}
