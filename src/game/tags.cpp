#include "tags.hpp"

#include <sf2/sf2.hpp>

namespace mo {

	void Player_tag_comp::load(sf2::JsonDeserializer& state,
	                           asset::Asset_manager&) {
		state.read_virtual(
			sf2::vmember("id", _id)
		);
	}
	void Player_tag_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("id", _id)
		);
	}

}
