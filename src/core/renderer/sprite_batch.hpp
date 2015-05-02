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


namespace mo {
namespace renderer {

	class Sprite_batch {

	public:

		struct SpriteVertex {
			SpriteVertex(glm::vec4 vec, glm::vec2 uv_coords, const renderer::Texture* t) : tex(t){
				pos = glm::vec2(vec.x, vec.y);
				uv = uv_coords;
			}

			bool operator<(SpriteVertex const& other) const {
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
			const renderer::Texture& texture;
			glm::vec4 uv;
		};

		// Constructors
		Sprite_batch(asset::Asset_manager& asset_manager);

		// Methods
        void draw(const renderer::Camera& cam, const Sprite& sprite) noexcept;
		void drawAll(const renderer::Camera& cam) noexcept;


	private:

		renderer::Object _object;
		renderer::Shader_program _shader;
		renderer::Texture_ptr _texture;
		//asset::Ptr<renderer::Texture> _texture;

		mutable std::vector<SpriteVertex> _vertices;

	};

}
}
