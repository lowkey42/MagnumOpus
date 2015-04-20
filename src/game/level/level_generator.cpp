#include "level_generator.hpp"

namespace game {
namespace level {

	Level generate_level(core::asset::Asset_manager& assets, uint64_t seed,
								int depth, int difficulty) {
		// TODO[foe]: impl me

		return Level{Tile_type::floor_tile, 64, 64};
	}

}
}
