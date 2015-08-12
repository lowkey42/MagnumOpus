#define MO_BUILD_SERIALIZER

#include "explosive_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	struct Explosive_comp::Persisted_state {
		float damage;
		float range;
		float delay;
		bool on_contact;
		bool on_damage;
		float explode_after;
		float blast_force;
		level::Element damage_type;
		Effect_type effect;
		Damage_effect damage_effect;
		float force_feedback;

		Persisted_state(const Explosive_comp& c)
			: damage(c._damage),
			  range(c._range / 1_m),
			  delay(c._delay / 1_s),
			  on_contact(c._activate_on_contact),
			  on_damage(c._activate_on_damage),
			  explode_after(c._delay_left/1_s),
			  blast_force(c._blast_force.value()),
			  damage_type(c._damage_type),
			  effect(c._explosion_effect),
			  damage_effect(c._damage_effect),
			  force_feedback(c._force_feedback) {}
	};

	sf2_structDef(Explosive_comp::Persisted_state,
		sf2_member(damage),
		sf2_member(range),
		sf2_member(delay),
		sf2_member(on_contact),
		sf2_member(on_damage),
		sf2_member(explode_after),
		sf2_member(blast_force),
		sf2_member(damage_type),
		sf2_member(effect),
		sf2_member(damage_effect),
		sf2_member(force_feedback)
	)

	void Explosive_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_damage = s.damage;
		_range = s.range * 1_m;
		_delay = s.delay * 1_s;
		_activate_on_contact = s.on_contact;
		_activate_on_damage = s.on_damage;
		_delay_left = s.explode_after * 1_s;
		_blast_force = s.blast_force * 1_n;
		_damage_type = s.damage_type;
		_explosion_effect = s.effect;
		_damage_effect = s.damage_effect;
		_force_feedback = s.force_feedback;
	}
	void Explosive_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
