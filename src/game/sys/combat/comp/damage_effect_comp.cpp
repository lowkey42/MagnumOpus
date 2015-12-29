#define MO_BUILD_SERIALIZER
#include "damage_effect_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	void Damage_effect_comp::load(sf2::JsonDeserializer& state,
	                              asset::Asset_manager&) {
		float left_f = _time_left / 1_s;

		state.read_virtual(
			sf2::vmember("type", _type),
			sf2::vmember("left", left_f),
			sf2::vmember("confusion", _confusion)
		);

		_time_left = left_f * 1_s;
	}
	void Damage_effect_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("type", _type),
			sf2::vmember("left", _time_left / 1_s),
			sf2::vmember("confusion", _confusion)
		);
	}

}
}
}
