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
#include "animation.hpp"
#include "../asset/aid.hpp"

#include "../../core/units.hpp"

#include <vector>


namespace mo {
namespace renderer {

	class Sprite_batch {

	public:

		struct Sprite_vertex {
			Sprite_vertex(glm::vec4 vec, glm::vec2 uv_coords, const renderer::Texture* t) : tex(t){
				pos = glm::vec2(vec.x, vec.y);
				uv = uv_coords;
			}

			bool operator<(Sprite_vertex const& other) const {
				//std::cout << "this tex: " << this->tex << " | other tex: " << other.tex << std::endl;
				return (this->tex < other.tex);
			}

			glm::vec2 pos;
			glm::vec2 uv;
			const renderer::Texture* tex;
		};

		struct Sprite{
			Position position;
			float rotation;
			const glm::vec4 uv;
			asset::Ptr<renderer::Animation> anim;
		};

		// Constructors
		Sprite_batch(asset::Asset_manager& asset_manager);

		// Methods
        void draw(const renderer::Camera& cam, const Sprite& sprite) noexcept;
		void draw_part(const std::vector<Sprite_vertex>::const_iterator begin, const std::vector<Sprite_vertex>::const_iterator end);
		void drawAll(const renderer::Camera& cam) noexcept;


	private:

		mutable std::vector<Sprite_vertex> _vertices;

		renderer::Object _object;
		renderer::Shader_program _shader;


	};

}
}
