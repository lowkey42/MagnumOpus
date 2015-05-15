/**************************************************************************\
 * load & save JSON files from Tiled editor                               *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "../../core/utils/template_utils.hpp"
#include "../../core/utils/string_utils.hpp"


namespace mo {
namespace level {

	struct TiledObject {
		int height;
		int width;
		int x;
		int y;
		int rotation;
		std::string name;
		std::string type;
		bool visible;
		std::unordered_map<std::string, std::string> properties;

	};

	struct TiledLayer {
		std::vector<int> data;
		int height;
		int width;
		std::string name;
		int opacity;
		std::string type;
		bool visible;
		int x;
		int y;

		std::string draworder = "topdown";
		std::vector<TiledObject> objects;
	};

	struct TiledLevel {
		int height;
		int width;
		std::vector<TiledLayer> layers;
		std::unordered_map<std::string, std::string> properties;

		std::string orientation = "orthogonal";
		std::string renderorder = "right-down";
		int tileheight;
		int tilewidth;
		int version;

		TiledLayer& add_layer(std::string name, std::string type) {
			layers.emplace_back();
			auto& l = layers.back();
			l.height = height;
			l.width = width;
			l.x = 0;
			l.y = 0;
			l.visible = true;
			l.name = std::move(name);
			l.opacity = 1;
			l.type = std::move(type);

			return l;
		}
	};

	extern TiledLevel parse_level(std::istream& s);
	extern void write_level(std::ostream& s, const TiledLevel& level);

}
}
