#include "drop_comp.hpp"

namespace mo {
namespace sys {
namespace item {

	void Drop_comp::load(sf2::JsonDeserializer& state,
	                     asset::Asset_manager&) {
		state.read_virtual(
			sf2::vmember("group", _group)
		);
	}
	void Drop_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("group", _group)
		);
	}

}
}
}
