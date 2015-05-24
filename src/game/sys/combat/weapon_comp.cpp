#include "weapon_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	struct Weapon_comp::Persisted_state {
		Weapon_type type;
		std::string bullet_type;
		float bullet_velocity;

		float cooldown;
		float cooldown_left;

		float melee_damage;
		float melee_range;
		float melee_angle;

		float attack_delay;

		float recoil;

		Persisted_state(const Weapon_comp& c)
				: type(c._type),
				  bullet_type(c._bullet_type ? c._bullet_type.name() : ""),
				  bullet_velocity(c._bullet_vel / (1_km/hour)),
				  cooldown(c._cooldown.value()),
				  cooldown_left(c._cooldown_left.value()),
				  melee_damage(c._melee_damage),
				  melee_range(c._melee_range.value()),
				  melee_angle(c._melee_angle / (1_deg).value()),
				  attack_delay(c._attack_delay.value()),
				  recoil(c._recoil.value()) {}
	};

	sf2_enumDef(Weapon_type, sf2_value(range), sf2_value(melee))

	sf2_structDef(Weapon_comp::Persisted_state,
		sf2_member(type),
		sf2_member(bullet_type),
		sf2_member(bullet_velocity),
		sf2_member(cooldown),
		sf2_member(cooldown_left),
		sf2_member(melee_damage),
		sf2_member(melee_range),
		sf2_member(melee_angle),
		sf2_member(attack_delay),
		sf2_member(recoil)
	)

	void Weapon_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_type = s.type;
		_bullet_vel = s.bullet_velocity * (1_km/hour);
		if(!s.bullet_type.empty())
			_bullet_type = asset::AID{asset::Asset_type::blueprint, s.bullet_type};

		_cooldown = s.cooldown * 1_s;
		_cooldown_left = s.cooldown_left * 1_s;
		_melee_damage = s.melee_damage;
		_melee_range = s.melee_range * 1_m;
		_melee_angle = s.melee_angle * 1_deg;
		_attack_delay = s.attack_delay * 1_s;
		_recoil = s.recoil * 1_N;
	}
	void Weapon_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
