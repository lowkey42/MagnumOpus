#include "level_generator.hpp"

namespace game {
namespace level {

	Level generate_level(core::asset::Asset_manager& assets, uint64_t seed,
								int depth, int difficulty) {
		// TODO[foe]: impl me

		constexpr auto size = 16;

		auto l = Level{Tile_type::floor_tile, size, size};

		for(int i=0; i<size; ++i)
			l.get(i,0).type = l.get(0,i).type = l.get(size-1,i).type = l.get(i, size-1).type = Tile_type::wall_dirt;

		l.get(size/2, size/2).type = Tile_type::wall_dirt;

		return l;
	}

}
}
