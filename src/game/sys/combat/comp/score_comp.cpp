#include "score_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	void Score_comp::load(sf2::JsonDeserializer& state,
	                      asset::Asset_manager&) {
		state.read_virtual(
			sf2::vmember("value", _value)
		);
	}
	void Score_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("value", _value)
		);
	}

}
}
}
