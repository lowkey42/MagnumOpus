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

#include <core/engine.hpp>
#include <core/renderer/camera.hpp>                  // TODO[sebastian]: replace with forward declaration
#include <core/renderer/shader.hpp>
#include <core/renderer/vertex_object.hpp>
#include <core/renderer/texture.hpp>                 // TODO[sebastian]: replace with forward declaration

#include <vector>
#include <glm/glm.hpp>

namespace mo {
namespace level {

	class Level;

	class Tilemap{

	public:

		struct TileVertex {
			glm::vec2 pos;
			glm::vec2 uv;
			float layer;
		};

		// Constructors
		Tilemap(Engine &engine, const Level &lev);

		// Methods
		void draw(const renderer::Camera& cam);

	private:

		// TODO automatic calculation with texture width & height
		uint16_t _tex_width = 256;
		uint16_t _tex_height = 128;
		uint16_t _tile_tex_width = 16;
		uint16_t _tile_tex_height = 16;
		// texture tiles per line (width / tile width)
		uint16_t _tpl = _tex_width / _tile_tex_width;

		double _xTexTile = _tile_tex_width / static_cast<double>(_tex_width);
		double _yTexTile = _tile_tex_height / static_cast<double>(_tex_height);

		const Level &_level;
		std::vector<TileVertex> _vertices;

		renderer::Object _object;
		renderer::Shader_program _shader;
		asset::Ptr<renderer::Texture> _texture;

	};

}
}
