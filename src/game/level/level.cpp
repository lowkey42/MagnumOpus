#include "level.hpp"


namespace game {
namespace level {

	bool Tile::solid()const {
		switch(type) {
			case Tile_type::floor_tile:
			case Tile_type::floor_stone:
			case Tile_type::floor_dirt:
			case Tile_type::stairs_up:
			case Tile_type::stairs_down:
			case Tile_type::door_open_ns:
			case Tile_type::door_open_we:
				return false;

			case Tile_type::indestructible_wall:
			case Tile_type::wall_tile:
			case Tile_type::wall_stone:
			case Tile_type::wall_dirt:
			case Tile_type::door_closed_ns:
			case Tile_type::door_closed_we:
				return true;

			default:
				return true;
		}
	}
	float Tile::friction()const {
		return 1.0f;
	}

	Level::Level(Tile_type default_type, int width, int height)
		: _width(width), _height(height), _tiles(width*height, Tile{default_type, Elements{}}) {
	}

	Level::Level(int width, int height, std::vector<Tile> data)
		: _width(width), _height(height), _tiles(data) {
	}

	Level::Level(std::istream& stream)
		: Level(Tile_type::floor_stone, 42, 42) {
		// TODO
	}
	Level::Level() : _width(0), _height(0) {
	}

	void Level::store(std::ostream& stream) {
		// TODO
	}

}
}
