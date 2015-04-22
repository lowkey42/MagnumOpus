#include "level_generator.hpp"

namespace game {
namespace level {

	Level generate_level(core::asset::Asset_manager& assets, uint64_t seed,
								int depth, int difficulty) {
		// TODO[foe]: impl me

		auto l = Level{Tile_type::floor_tile, 64, 64};

		for(int i=0; i<64; ++i)
			l.get(i,0).type = l.get(0,i).type = l.get(63,i).type = l.get(i, 63).type = Tile_type::wall_dirt;

		return l;
	}

}
}
