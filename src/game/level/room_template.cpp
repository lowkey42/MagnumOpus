#include "room_template.hpp"


namespace game {
namespace level {

	Room_template::Room_template(Tile_type default_type, int width, int height)
		: Level(default_type, width, height) {
	}

	Room_template::Room_template(int width, int height, std::vector<Tile> data, std::vector<Room_tile_props> props, std::vector<Room_object_props> objs)
		: Level(width, height, data), _props(props), _objs(objs) {
	}

	void Room_template::_store(TiledLevel& l)const {
		// TODO: ?
	}
	void Room_template::_load(const TiledLevel& l) {
		_props.resize(_tiles.size());

		// TODO: load properties & objects

	}

}
}
