#include "level.hpp"

#include "../../core/utils/template_utils.hpp"
#include "../../core/utils/string_utils.hpp"

#include "tiled.hpp"

namespace mo {
namespace level {

	using namespace mo::util;

	auto Tile::solid()const -> bool {
		switch(type) {
			case Tile_type::floor_tile:
			case Tile_type::floor_stone:
			case Tile_type::floor_dirt:
			case Tile_type::door_open_ns:
			case Tile_type::door_open_we:
				return false;

			case Tile_type::indestructible_wall:
			case Tile_type::wall_tile:
			case Tile_type::wall_stone:
			case Tile_type::wall_dirt:
				return true;

			case Tile_type::stairs_up:
			case Tile_type::stairs_down:
			case Tile_type::door_closed_ns:
			case Tile_type::door_closed_we:
				return true;
		}
	}
	auto Tile::height()const -> float {
		switch(type) {
			case Tile_type::floor_tile:
			case Tile_type::floor_stone:
			case Tile_type::floor_dirt:
			case Tile_type::door_open_ns:
			case Tile_type::door_open_we:
				return 0.f;

			case Tile_type::stairs_up:
			case Tile_type::stairs_down:
				return 0.1f;

			case Tile_type::indestructible_wall:
			case Tile_type::wall_tile:
			case Tile_type::wall_stone:
			case Tile_type::wall_dirt:
			case Tile_type::door_closed_ns:
			case Tile_type::door_closed_we:
				return 0.9f;
		}
	}
	auto Tile::solid(float x, float y)const -> bool {
		if(solid())
			return x>=dimensions().x && x<dimensions().z &&
			       y>=dimensions().y && y<dimensions().w;

		return false;
	}
	auto Tile::dimensions()const -> glm::vec4 {
		switch(type) {

			case Tile_type::stairs_up:
				return {-.5f,-.5f, -.2f,.5f};
			case Tile_type::stairs_down:
				return {.2f,-.5f, .5f,.5f};

			case Tile_type::door_closed_ns:
				// TODO
			case Tile_type::door_closed_we:
				// TODO

			default:
				return {-.5f,-.5f, .5f,.5f};
		}

	}
	float Tile::friction()const {
		return 1.0f;
	}

	Level::Level(Tile_type default_type, int width, int height, std::vector<Room> rooms)
		: _width(width), _height(height), _tiles(width*height, Tile{default_type, Elements{}}), _rooms(rooms) {
	}

	Level::Level(int width, int height, std::vector<Tile> data, std::vector<Room> rooms)
		: _width(width), _height(height), _tiles(data), _rooms(rooms) {
	}

	Level::Level() : _width(0), _height(0) {
	}

	auto Level::find_room(Room_type type)const -> maybe<const Room&> {
		auto found = std::find_if(_rooms.begin(), _rooms.end(), [type](auto& r){return r.type==type;});

		return found!=_rooms.end() ? just(*found) : nothing();
	}

	void Level::load(std::istream& stream) {
		const TiledLevel level_data = parse_level(stream);

		_width = level_data.width;
		_height = level_data.height;
		_tiles.reserve(_width*_height);

		auto layer = std::find_if(begin(level_data.layers), end(level_data.layers),
		                          [](const TiledLayer& l){return l.name=="main";} );

		INVARIANT(layer!=level_data.layers.end(), "No 'main'-Layer in file.");

		for(const auto& d : layer->data) {
			INVARIANT(d>=0 && static_cast<std::size_t>(d)<tile_type_count, "Invalid tile_type");
			_tiles.push_back(Tile{static_cast<Tile_type>(d), Elements{}});
		}

		for(auto& l : level_data.layers) {
			if(l.type=="tilelayer" && starts_with(l.name, "element")) {
				int i=0;
				for(const auto& d : l.data) {
					if(d>0 && static_cast<std::size_t>(d)<element_count)
						_tiles[i++].elements |= static_cast<Element>(d);
				}
			}
		}

		_load(level_data);
	}
	void Level::store(std::ostream& stream)const {
		TiledLevel level_data;
		level_data.width = _width;
		level_data.height = _height;
		level_data.version = 1;
		level_data.tileheight = 16;
		level_data.tilewidth= 16;
		auto& tile_layer = level_data.add_layer("main", "tilelayer");

		auto max_elements = 0ul;
		for(auto& t : _tiles) {
			tile_layer.data.push_back(static_cast<int>(t.type));
			max_elements = std::max(max_elements, static_cast<unsigned long>(t.elements.size()));
		}

		for(auto i : range(max_elements)) {
			auto& element_layer = level_data.add_layer("element-" + to_string(i), "tilelayer");
			for(auto& t : _tiles) {
				element_layer.data.push_back(static_cast<int>(t.elements[i]));
			}
		}

		_store(level_data);

		write_level(stream, level_data);
	}

}
}
