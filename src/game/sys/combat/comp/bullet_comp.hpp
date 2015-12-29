/**************************************************************************\
 * The component that causes pain! deals damage on contact                *
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

#include <core/ecs/ecs.hpp>
#include <core/units.hpp>

#include "../../../level/elements.hpp"
#include "../../../effects.hpp"

#include "damage_effect_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	class Bullet_comp : public ecs::Component<Bullet_comp> {
		public:
			static constexpr const char* name() {return "Bullet";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Bullet_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}
			Bullet_comp(Bullet_comp&&)noexcept = default;
			~Bullet_comp()noexcept = default;
			Bullet_comp& operator=(Bullet_comp&&)noexcept = default;

		private:
			friend class Combat_system;
			friend class Reaper_subsystem;

			float _damage = 1;
			level::Element _damage_type   = level::Element::neutral;

			int _break_after_entities = 1;
			int _break_after_walls = 1;

			Effect_type   _effect         = Effect_type::none;
			Damage_effect _damage_effect  = Damage_effect::none;
	};

}
}
}
