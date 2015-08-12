/**************************************************************************\
 * stores the current health of an entity (+shield, ...)                  *
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

namespace mo {
namespace sys {
namespace combat {

	class Health_comp : public ecs::Component<Health_comp> {
		public:
			static constexpr const char* name() {return "Health";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Health_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			void heal(float hp)noexcept {_heal+=hp;}
			void damage(float hp, level::Element type=level::Element::neutral)noexcept;

			auto hp()const noexcept {return _current_hp;}
			auto max_hp()const noexcept {return _max_hp;}
			auto hp_percent()const noexcept {return _current_hp/_max_hp;}

			auto damaged()const noexcept {return _current_hp<_max_hp;}

			auto death_effect()const noexcept {return _death_effect;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Combat_system;
			friend class Reaper_subsystem;

			float _auto_heal_max=0;
			float _auto_heal=0;

			float _max_hp = 0;
			float _current_hp = 0;

			float _damage=0, _heal=0;

			float _physical_resistence = 0.f;
			level::Elements _resistences;
			level::Elements _vulnerabilities;

			Effect_type _death_effect = Effect_type::none;

			float _death_force_feedback = 0;
	};

}
}
}
