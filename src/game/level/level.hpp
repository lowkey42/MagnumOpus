/**************************************************************************\
 * basic structure of the known universe                                  *
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
#include <iostream>

#include "elements.hpp"
#include "../../core/asset/asset_manager.hpp"
#include "../../core/utils/template_utils.hpp"

namespace game {
namespace level {

	enum class Tile_type {
		indestructible_wall,

		floor_tile,
		floor_stone,
		floor_dirt,

		wall_tile,
		wall_stone,
		wall_dirt,

		door_open_ns,
		door_closed_ns,
		door_open_we,
		door_closed_we,

		stairs_up,
		stairs_down
	};

	struct Tile {
		Tile_type type;
		Elements elements;

		bool solid()const;
		float friction()const;
		void toggle();
	};

	// currently played part of the world
	class Level {
		public:
			Level(Tile_type default_type, int width, int height);
			Level(int width, int height, std::vector<Tile> data);
			Level(std::istream& stream);
			Level();

			void store(std::ostream& stream);

			auto& get(int x, int y)      {return _tiles.at(y*_width + x);}
			auto& get(int x, int y)const {return _tiles.at(y*_width + x);}

			// handler = void(int x, int y, const Tile& tile)
			template<typename F>
			void foreach_tile(int min_x, int min_y, int max_x, int max_y, F handler)const;

			bool  solid(int x, int y)   const {return get(x,y).solid();}
			float friction(int x, int y)const {return get(x,y).friction();}

			int width() const noexcept {return _width;}
			int height()const noexcept {return _height;}

		protected:
			int _width;
			int _height;
			std::vector<Tile> _tiles;
	};

	template<typename F>
	void Level::foreach_tile(int min_x, int min_y, int max_x, int max_y, F handler)const {
		using core::util::range;

		min_x = std::max(0, min_x);
		min_y = std::max(0, min_y);
		max_x = std::min(_width,  max_x-1);
		max_y = std::min(_height, max_y-1);

		for(auto y : range(min_y, max_y)) {
			for(auto x : range(min_x, max_x)) {
				handler(x,y, _tiles[y*_width + x]);
			}
		}
	}

}
}

namespace core {
namespace asset {
	template<>
	struct Loader<game::level::Level> {
		using RT = std::shared_ptr<const game::level::Level>;

		RT operator()(istream in) throw(Loading_failed){
			return std::make_shared<game::level::Level>(in);
		}

		void operator()(ostream out, game::level::Level& level) throw(Loading_failed) {
			level.store(out);
		}
	};
}
}
