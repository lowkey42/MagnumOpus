#include "friend_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	void Friend_comp::load(sf2::JsonDeserializer& state,
	                       asset::Asset_manager&) {
		state.read_virtual(
			sf2::vmember("group", _group)
		);
	}
	void Friend_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("group", _group)
		);
	}

}
}
}
