#define MO_BUILD_SERIALIZER

#include "bullet_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	struct Bullet_comp::Persisted_state {
		float damage;
		level::Element damage_type = level::Element::neutral;

		int break_after_entities=1;
		int break_after_walls=1;

		Effect_type effect = Effect_type::none;
		Damage_effect damage_effect = Damage_effect::none;

		Persisted_state(const Bullet_comp& c)
			: damage(c._damage),
			  damage_type(c._damage_type),
			  break_after_entities(c._break_after_entities),
			  break_after_walls(c._break_after_walls),
			  effect(c._effect),
			  damage_effect(c._damage_effect) {}
	};

	sf2_structDef(Bullet_comp::Persisted_state,
		sf2_member(damage),
		sf2_member(damage_type),
		sf2_member(break_after_entities),
		sf2_member(break_after_walls),
		sf2_member(effect),
		sf2_member(damage_effect)
	)

	void Bullet_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_damage = s.damage;
		_damage_type = s.damage_type;
		_break_after_entities = s.break_after_entities;
		_break_after_walls = s.break_after_walls;
		_effect = s.effect;
		_damage_effect = s.damage_effect;
	}
	void Bullet_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
