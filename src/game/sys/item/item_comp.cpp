#define MO_BUILD_SERIALIZER
#include "item_comp.hpp"


namespace mo {
namespace sys {
namespace item {

	sf2_enumDef(Item_target,
		health,
		score,
		element,
		bullet_time
	)

	void Item_comp::load(sf2::JsonDeserializer& state,
	                     asset::Asset_manager&) {
		state.read_virtual(
			sf2::vmember("target", _target),
			sf2::vmember("element", _element),
			sf2::vmember("mod", _mod),
			sf2::vmember("joinable", _joinable)
		);
	}
	void Item_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("target", _target),
			sf2::vmember("element", _element),
			sf2::vmember("mod", _mod),
			sf2::vmember("joinable", _joinable)
		);
	}

}
}
}
