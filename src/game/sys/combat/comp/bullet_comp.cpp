#define MO_BUILD_SERIALIZER

#include "bullet_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	void Bullet_comp::load(sf2::JsonDeserializer& state,
	                       asset::Asset_manager&) {
		state.read_virtual(
			sf2::vmember("damage", _damage),
			sf2::vmember("damage_type", _damage_type),
			sf2::vmember("break_after_entities", _break_after_entities),
			sf2::vmember("break_after_walls", _break_after_walls),
			sf2::vmember("effect", _effect),
			sf2::vmember("damage_effect", _damage_effect)
		);
	}
	void Bullet_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("damage", _damage),
			sf2::vmember("damage_type", _damage_type),
			sf2::vmember("break_after_entities", _break_after_entities),
			sf2::vmember("break_after_walls", _break_after_walls),
			sf2::vmember("effect", _effect),
			sf2::vmember("damage_effect", _damage_effect)
		);
	}

}
}
}
