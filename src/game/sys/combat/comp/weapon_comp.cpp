#define MO_BUILD_SERIALIZER

#include "weapon_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;


	struct Laser_sight_comp::Persisted_state {
		uint8_t r,g,b, occlusion;
		float width;

		Persisted_state(const Laser_sight_comp& o)
		    : r(o._color.r*255), g(o._color.g*255),
		      b(o._color.b*255), occlusion(o._color.a*255),
		      width(o._width){
		}
	};
	sf2_structDef(Laser_sight_comp::Persisted_state,
		sf2_member(r),
		sf2_member(g),
		sf2_member(b),
		sf2_member(occlusion),
		sf2_member(width)
	)

	void Laser_sight_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});

		_color = {s.r/255.f, s.g/255.f, s.b/255.f, s.occlusion/255.f};
		_width = s.width;
	}
	void Laser_sight_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}



	struct Weapon_comp::Persisted_state {
		Weapon::Persisted_state weapon;

		float cooldown_left;

		Persisted_state(const Weapon_comp& c)
		    : weapon(c._weapon),
		      cooldown_left(c._cooldown_left.value()) {}
	};

	sf2_structDef(Weapon_comp::Persisted_state,
		sf2_member(weapon),
		sf2_member(cooldown_left)
	)

	void Weapon_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_weapon = s.weapon.to_weapon();
		_cooldown_left = s.cooldown_left * 1_s;
	}
	void Weapon_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
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
