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
#include "../sys/physics/physics_system.hpp"

namespace game {
namespace level {
	struct TiledLevel;

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
	constexpr auto tile_type_count = static_cast<std::size_t>(Tile_type::stairs_down)+1;

	struct Tile {
		Tile_type type;
		Elements elements;

		bool solid()const;
		float friction()const;
		void toggle();
	};

	enum class Room_type {
		normal,
		start,
		end,
		boss,
		secret
	};
	struct Room {
		std::size_t id;
		int top, left, right, bottom;
		Room_type type = Room_type::normal;
	//	std::vector<Room*> connections;

		Room() = default;
		Room(int top, int left, int right, int bottom)
		    : top(top), left(left), right(right), bottom(bottom) {}
	};

	class Level : public sys::physics::World_interface {
		public:
			Level(Tile_type default_type, int width, int height);
			Level(int width, int height, std::vector<Tile> data);
			Level();

			void store(std::ostream& stream)const;
			void load(std::istream& stream);

			auto& get(int x, int y)      {return _tiles.at(y*_width + x);}
			auto& get(int x, int y)const {return _tiles.at(y*_width + x);}

			// handler = void(int x, int y, const Tile& tile)
			template<typename F>
			void foreach_tile(int min_x, int min_y, int max_x, int max_y, F handler)const;

			auto solid   (int x, int y)const -> bool  override {return get(x,y).solid();}
			auto friction(int x, int y)const -> float override {return get(x,y).friction();}

			auto width()  const noexcept     -> int   override {return _width;}
			auto height() const noexcept     -> int   override {return _height;}

		protected:
			virtual void _store(TiledLevel&)const {}
			virtual void _load(const TiledLevel&) {}

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
			auto l = std::make_shared<game::level::Level>();
			l->load(in);
			return l;
		}

		void operator()(ostream out, const game::level::Level& level) throw(Loading_failed) {
			level.store(out);
		}
	};
}
}
