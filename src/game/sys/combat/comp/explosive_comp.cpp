#define MO_BUILD_SERIALIZER

#include "explosive_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	void Explosive_comp::load(sf2::JsonDeserializer& state,
	                          asset::Asset_manager&) {
		float range_f = _range / 1_m;
		float delay_f = _delay / 1_s;
		float explode_after_f = _delay_left / 1_s;
		float blast_force_f = _blast_force / 1_n;

		state.read_virtual(
			sf2::vmember("damage", _damage),
			sf2::vmember("range", range_f),
			sf2::vmember("delay", delay_f),
			sf2::vmember("on_contact", _activate_on_contact),
			sf2::vmember("on_damage", _activate_on_damage),
			sf2::vmember("explode_after", explode_after_f),
			sf2::vmember("blast_force", blast_force_f),
			sf2::vmember("damage_type", _damage_type),
			sf2::vmember("effect", _explosion_effect),
			sf2::vmember("damage_effect", _damage_effect),
			sf2::vmember("force_feedback", _force_feedback)
		);

		_range = range_f * 1_m;
		_delay = delay_f * 1_s;
		_delay_left = explode_after_f * 1_s;
		_blast_force = blast_force_f * 1_n;
	}
	void Explosive_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("damage", _damage),
			sf2::vmember("range", _range / 1_m),
			sf2::vmember("delay", _delay / 1_s),
			sf2::vmember("on_contact", _activate_on_contact),
			sf2::vmember("on_damage", _activate_on_damage),
			sf2::vmember("explode_after", _delay_left / 1_s),
			sf2::vmember("blast_force", _blast_force / 1_n),
			sf2::vmember("damage_type", _damage_type),
			sf2::vmember("effect", _explosion_effect),
			sf2::vmember("damage_effect", _damage_effect),
			sf2::vmember("force_feedback", _force_feedback)
		);
	}

}
}
}
