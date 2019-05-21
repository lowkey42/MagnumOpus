#define MO_BUILD_SERIALIZER

#include "weapon_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	void Laser_sight_comp::load(sf2::JsonDeserializer& state,
	                            asset::Asset_manager&) {
		float r=_color.r*255,g=_color.g*255,b=_color.b*255,a=_color.a*255;

		state.read_virtual(
			sf2::vmember("r", r),
			sf2::vmember("g", g),
			sf2::vmember("b", b),
			sf2::vmember("occlusion", a),
			sf2::vmember("width", _width)
		);

		_color = {r/255.f, g/255.f, b/255.f, a/255.f};
	}
	void Laser_sight_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("r", _color.r * 255),
			sf2::vmember("g", _color.g * 255),
			sf2::vmember("b", _color.b * 255),
			sf2::vmember("occlusion", _color.a * 255),
			sf2::vmember("width", _width)
		);
	}

	void Weapon_comp::load(sf2::JsonDeserializer& state,
	                       asset::Asset_manager&) {
		float cooldown_left = _cooldown_left / 1_s;

		state.read_virtual(
			sf2::vmember("weapon", _weapon),
			sf2::vmember("cooldown_left", cooldown_left)
		);

		_cooldown_left = cooldown_left * 1_s;
	}
	void Weapon_comp::save(sf2::JsonSerializer& state)const {
		float cooldown_left = _cooldown_left / 1_s;

		state.write_virtual(
			sf2::vmember("weapon", _weapon),
			sf2::vmember("cooldown_left", cooldown_left)
		);
	}

	Weapon_modifier::Weapon_modifier(
	        Weapon_modifier_collection& collection,
	        int8_t priority)
	    : _collection(collection), _priority(priority) {
		_collection.register_modifier(*this);
	}

	Weapon_modifier::~Weapon_modifier()noexcept {
		_collection.unregister_modifier(*this);
	}

	void Weapon_modifier_collection::register_modifier(Weapon_modifier& m) {
		_modifiers.push_back(&m);
		std::sort(_modifiers.begin(), _modifiers.end(),
		          [](auto& a, auto& b){return a->_priority>b->_priority;});
	}

	void Weapon_modifier_collection::unregister_modifier(Weapon_modifier& m) {
		util::erase_fast_stable(_modifiers, &m);
	}

	void Weapon_modifier_collection::on_attack(ecs::Entity& e, const Weapon& w) {
		for(auto& m : _modifiers) {
			m->on_attack(e,w);
		}
	}
}
}
}
