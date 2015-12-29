#include "tiled.hpp"

#include "../../core/utils/template_utils.hpp"
#include "../../core/utils/string_utils.hpp"

#include <sf2/sf2.hpp>

namespace mo {
namespace level {

	sf2_structDef(TiledObject,
		height,
		width,
		x,
		y,
		rotation,
		name,
		type,
		visible,
		properties
	)
	sf2_structDef(TiledLayer,
		data,
		height,
		width,
		name,
		opacity,
		type,
		visible,
		x,
		y,
		draworder,
		objects
	)
	sf2_structDef(TiledLevel,
		height,
		width,
		layers,
		properties,
		orientation,
		renderorder,
		tileheight,
		tilewidth,
		version
	)

	TiledLevel parse_level(std::istream& in) {
		TiledLevel level_data;
		sf2::deserialize_json(in, level_data);

		return level_data;
	}

	void write_level(std::ostream& out, const TiledLevel& level) {
		sf2::serialize_json(out, level);
	}

}
}
