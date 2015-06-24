/**************************************************************************\
 * tilemap.hpp - drawing and positioning a tilemap of a given level       *
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

#include <core/renderer/shader.hpp>
#include <core/renderer/vertex_object.hpp>
#include <core/renderer/texture.hpp>

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mo {
	class Engine;
	namespace renderer{class Camera;}

namespace level {

	class Level;


	struct TileVertex {
		glm::vec3 pos;
		glm::vec2 uv;
		TileVertex(glm::vec3 pos, glm::vec2 uv)
		    : pos(pos), uv(uv) {}
	};

	class Tilemap {
		public:
			Tilemap(Engine &engine, const Level &lev);

			void draw(const renderer::Camera& cam);

		private:
			const Level &_level;
			std::vector<TileVertex> _vertices;

			renderer::Object _object;
			renderer::Shader_program _shader;
			renderer::Texture_ptr _texture;
	};

}
}
