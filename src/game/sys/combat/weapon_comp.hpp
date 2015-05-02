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

namespace mo {
namespace sys {
namespace combat {

	enum class Weapon_type {
		range,
		melee
	};

	class Weapon_comp : public ecs::Component<Weapon_comp> {
		public:
			static constexpr const char* name() {return "Weapon";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Weapon_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			auto weapon_type()const noexcept{return _type;}

			void attack()noexcept{_attack = true;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Combat_system;

			Weapon_type _type        = Weapon_type::melee;
			asset::AID  _bullet_type = asset::AID{};
			Speed       _bullet_vel  = Speed{0};

			Time        _cooldown      = Time(1);
			Time        _cooldown_left = Time(0);
			bool        _attack        = false;

			float       _melee_damage  = 5;
			Distance    _melee_range   = Distance{1};
			Angle       _melee_angle   = Angle{3};
	};

}
}
}
