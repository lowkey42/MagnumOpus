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

#include "../../../level/elements.hpp"
#include "../../../effects.hpp"
#include "damage_effect_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using level::Element;

	class Laser_sight_comp : public ecs::Component<Laser_sight_comp> {
		public:
			static constexpr const char* name() {return "Laser_sight";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Laser_sight_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			auto color()const noexcept {return _color;}
			auto width()const noexcept {return _width;}

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

		Effect_type   effect        = Effect_type::none;
		Element       damage_type   = Element::neutral;
		Damage_effect damage_effect = Damage_effect::none;

		float       melee_damage  = 5;
		Distance    melee_range   = Distance{1};
		Angle       melee_angle   = Angle{3};

		Time        attack_delay  = Time(0);
		Time        cooldown      = Time(1);

		Force       recoil        = Force(0);

		float       fuel_usage    = 1.f;
		float       force_feedback=0.f;

		Weapon() = default;

		Weapon(Weapon_type type, asset::AID bullet_type, Speed bullet_vel, Angle spreading, int bullet_count,
		       Effect_type effect, Element damage_type, Damage_effect damage_effect, float melee_damage, Distance melee_range, Angle melee_angle,
		       Time attack_delay, Time cooldown, Force recoil, float fuel_usage, float force_feedback)
		    : type(type), bullet_type(bullet_type), bullet_vel(bullet_vel), spreading(spreading),
		      bullet_count(bullet_count), effect(effect), damage_type(damage_type), damage_effect(damage_effect), melee_damage(melee_damage),
		      melee_range(melee_range), melee_angle(melee_angle), attack_delay(attack_delay), cooldown(cooldown),
		      recoil(recoil), fuel_usage(fuel_usage), force_feedback(force_feedback) {}

	};

	class Weapon_comp : public ecs::Component<Weapon_comp> {
		public:
			static constexpr const char* name() {return "Weapon";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager&)override;
			void save(sf2::JsonSerializer& state)const override;

			Weapon_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			void attack()noexcept{_attack = true;}

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

	sf2_enumDef(Weapon_type, range, melee)

	inline void save(sf2::JsonSerializer& s, const Weapon& w) {
		std::string bullet_type = w.bullet_type ? w.bullet_type.name() : "";
		float bullet_velocity = w.bullet_vel / (1_km/hour);
		float spreading = w.spreading.in_degrees()*2;
		float melee_range = w.melee_range / 1_m;
		float melee_angle = w.melee_angle / 1_deg;
		float attack_delay = w.attack_delay / 1_s;
		float cooldown = w.cooldown / 1_s;
		float recoil = w.recoil / 1_n;

		s.write_virtual(
			sf2::vmember("type", w.type),
			sf2::vmember("bullet_type", bullet_type),
			sf2::vmember("bullet_velocity", bullet_velocity),
			sf2::vmember("spreading", spreading),
			sf2::vmember("bullet_count", w.bullet_count),
			sf2::vmember("effect", w.effect),
			sf2::vmember("damage_type", w.damage_type),
			sf2::vmember("damage_effect", w.damage_effect),
			sf2::vmember("melee_damage", w.melee_damage),
			sf2::vmember("melee_range", melee_range),
			sf2::vmember("melee_angle", melee_angle),
			sf2::vmember("attack_delay", attack_delay),
			sf2::vmember("cooldown", cooldown),
			sf2::vmember("recoil", recoil),
			sf2::vmember("fuel_usage", w.fuel_usage),
			sf2::vmember("force_feedback", w.force_feedback)
		);
	}

	inline void load(sf2::JsonDeserializer& s, Weapon& w) {
		std::string bullet_type = w.bullet_type ? w.bullet_type.name() : "";
		float bullet_velocity = w.bullet_vel / (1_km/hour);
		float spreading = w.spreading.in_degrees()*2;
		float melee_range = w.melee_range / 1_m;
		float melee_angle = w.melee_angle / 1_deg;
		float attack_delay = w.attack_delay / 1_s;
		float cooldown = w.cooldown / 1_s;
		float recoil = w.recoil / 1_n;

		s.read_virtual(
			sf2::vmember("type", w.type),
			sf2::vmember("bullet_type", bullet_type),
			sf2::vmember("bullet_velocity", bullet_velocity),
			sf2::vmember("spreading", spreading),
			sf2::vmember("bullet_count", w.bullet_count),
			sf2::vmember("effect", w.effect),
			sf2::vmember("damage_type", w.damage_type),
			sf2::vmember("damage_effect", w.damage_effect),
			sf2::vmember("melee_damage", w.melee_damage),
			sf2::vmember("melee_range", melee_range),
			sf2::vmember("melee_angle", melee_angle),
			sf2::vmember("attack_delay", attack_delay),
			sf2::vmember("cooldown", cooldown),
			sf2::vmember("recoil", recoil),
			sf2::vmember("fuel_usage", w.fuel_usage),
			sf2::vmember("force_feedback", w.force_feedback)
		);

		w.bullet_type = bullet_type.empty() ? asset::AID()
		                         : asset::AID{asset::Asset_type::blueprint,
		                                      bullet_type};
		w.bullet_vel = bullet_velocity * (1_km/hour);
		w.spreading = spreading/2 * 1_deg;
		w.melee_range = melee_range * 1_m;
		w.melee_angle = melee_angle * 1_deg;
		w.attack_delay = attack_delay * 1_s;
		w.cooldown = cooldown * 1_s;
		w.recoil = recoil * 1_n;
	}

}
}
}
#endif

