/**************************************************************************\
 * markes entities that can attack others                                 *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <core/asset/aid.hpp>
#include <core/ecs/ecs.hpp>
#include <core/units.hpp>

#include "../../../effects.hpp"

namespace mo {
namespace sys {
namespace combat {

	class Laser_sight_comp : public ecs::Component<Laser_sight_comp> {
		public:
			static constexpr const char* name() {return "Laser_sight";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Laser_sight_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			auto color()const noexcept {return _color;}
			auto width()const noexcept {return _width;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			glm::vec4 _color {1,0,0,0.2};
			float _width = 0.04;
	};


	enum class Weapon_type {
		range,
		melee
	};

	struct Weapon {
		Weapon_type type          = Weapon_type::melee;
		asset::AID  bullet_type   = asset::AID{};
		Speed       bullet_vel    = Speed{0};
		Angle       spreading     = Angle{0};
		int         bullet_count  = 1;

		Effect_type effect        = Effect_type::none;
		asset::AID  sound         = asset::AID{};

		float       melee_damage  = 5;
		Distance    melee_range   = Distance{1};
		Angle       melee_angle   = Angle{3};

		Time        attack_delay  = Time(0);
		Time        cooldown      = Time(1);

		Force       recoil        = Force(0);

		float       fuel_usage    = 1.f;

		Weapon() = default;
		~Weapon() = default;
		Weapon(const Weapon&) = default;
		Weapon& operator=(const Weapon&) = default;

		Weapon(Weapon_type type, asset::AID bullet_type, Speed bullet_vel, Angle spreading, int bullet_count,
		       Effect_type effect, asset::AID sound, float melee_damage, Distance melee_range, Angle melee_angle,
		       Time attack_delay, Time cooldown, Force recoil, float fuel_usage)
		    : type(type), bullet_type(bullet_type), bullet_vel(bullet_vel), spreading(spreading),
		      bullet_count(bullet_count), effect(effect), sound(sound), melee_damage(melee_damage),
		      melee_range(melee_range), melee_angle(melee_angle), attack_delay(attack_delay), cooldown(cooldown),
		      recoil(recoil), fuel_usage(fuel_usage){}

		struct Persisted_state;
	};

	class Weapon_comp : public ecs::Component<Weapon_comp> {
		public:
			static constexpr const char* name() {return "Weapon";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Weapon_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			void attack()noexcept{_attack = true;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Combat_system;

			Weapon _weapon;

			Time   _cooldown_left     = Time(0);
			Time   _attack_delay_left = Time(0);
			bool   _attack            = false;
	};

	class Weapon_modifier_collection;

	class Weapon_modifier {
		public:
			Weapon_modifier(Weapon_modifier_collection& collection,
							int8_t priority=0);
			virtual ~Weapon_modifier()noexcept;

			virtual void process(ecs::Entity& e, Weapon&) = 0;
			virtual void on_attack(ecs::Entity& e, const Weapon&) {}

		private:
			friend class Weapon_modifier_collection;

			Weapon_modifier_collection& _collection;
			const int8_t _priority;
	};
	class Weapon_modifier_collection {
		public:
			void register_modifier(Weapon_modifier& m);
			void unregister_modifier(Weapon_modifier& m);

			void on_attack(ecs::Entity& e, const Weapon&);

		protected:
			std::vector<Weapon_modifier*>  _modifiers;
	};

}
}
}

#ifdef MO_BUILD_SERIALIZER
#include <sf2/sf2.hpp>

namespace mo {
namespace sys {
namespace combat {
	using namespace unit_literals;

	struct Weapon::Persisted_state {
		Weapon_type type;
		std::string bullet_type;
		float bullet_velocity;
		float spreading = 0;
		int bullet_count = 1;

		Effect_type effect = Effect_type::none;
		std::string sound;

		float melee_damage = 0;
		float melee_range = 1;
		float melee_angle = 90;

		float attack_delay = 0;
		float cooldown = 1;

		float recoil = 0;

		float fuel_usage = 0.f;

		Persisted_state() = default;
		Persisted_state(const Weapon& c)
		    : type(c.type),
		      bullet_type(c.bullet_type ? c.bullet_type.name() : ""),
		      bullet_velocity(c.bullet_vel / (1_km/hour)),
		      spreading(c.spreading.in_degrees()*2),
		      bullet_count(c.bullet_count),
		      melee_damage(c.melee_damage),
		      melee_range(c.melee_range.value()),
		      melee_angle(c.melee_angle / (1_deg).value()),
		      attack_delay(c.attack_delay.value()),
		      cooldown(c.cooldown.value()),
		      recoil(c.recoil.value()),
		      fuel_usage(c.fuel_usage) {}

		auto to_weapon()const noexcept -> Weapon {
            return Weapon{
				type,
				bullet_type.empty() ? asset::AID()
				                    : asset::AID{asset::Asset_type::blueprint, bullet_type},
				bullet_velocity * (1_km/hour),
				spreading/2.f * 1_deg,
				bullet_count,
				effect,
				sound.empty() ? asset::AID()
				              : asset::AID{asset::Asset_type::sound, sound},
				melee_damage,
				melee_range * 1_m,
				melee_angle * 1_deg,
				attack_delay * 1_s,
				cooldown * 1_s,
				recoil * 1_n,
				fuel_usage
			};
		}
	};

	sf2_enumDef(Weapon_type, sf2_value(range), sf2_value(melee))

	sf2_structDef(Weapon::Persisted_state,
		sf2_member(type),
		sf2_member(bullet_type),
		sf2_member(bullet_velocity),
		sf2_member(spreading),
		sf2_member(bullet_count),
		sf2_member(effect),
		sf2_member(sound),
		sf2_member(cooldown),
		sf2_member(melee_damage),
		sf2_member(melee_range),
		sf2_member(melee_angle),
		sf2_member(attack_delay),
		sf2_member(recoil),
		sf2_member(fuel_usage)
	)
}
}
}
#endif

