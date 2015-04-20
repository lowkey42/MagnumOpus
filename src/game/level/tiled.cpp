#include "tiled.hpp"

#include "../../core/utils/template_utils.hpp"
#include "../../core/utils/string_utils.hpp"

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

namespace game {
namespace level {

	sf2_structDef(TiledObject,
		sf2_member(height),
		sf2_member(width),
		sf2_member(x),
		sf2_member(y),
		sf2_member(rotation),
		sf2_member(name),
		sf2_member(type),
		sf2_member(visible),
		sf2_member(properties)
	)
	sf2_structDef(TiledLayer,
		sf2_member(data),
		sf2_member(height),
		sf2_member(width),
		sf2_member(name),
		sf2_member(opacity),
		sf2_member(type),
		sf2_member(visible),
		sf2_member(x),
		sf2_member(y),
		sf2_member(draworder),
		sf2_member(objects)
	)
	sf2_structDef(TiledLevel,
		sf2_member(height),
		sf2_member(width),
		sf2_member(layers),
		sf2_member(properties),
		sf2_member(orientation),
		sf2_member(renderorder),
		sf2_member(tileheight),
		sf2_member(tilewidth),
		sf2_member(version)
	)

	TiledLevel parse_level(std::istream& s) {
		TiledLevel level_data;
		sf2::parseStream(s, level_data);

		return level_data;
	}

	void write_level(std::ostream& s, const TiledLevel& level) {
		sf2::writeStream(s, level);
	}

}
}
