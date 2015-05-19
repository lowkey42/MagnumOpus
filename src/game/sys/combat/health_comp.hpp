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
			void damage(float hp)noexcept{_damage+=hp;}

			auto hp()const noexcept {return _current_hp;}
			auto max_hp()const noexcept {return _max_hp;}

			auto damaged()const noexcept {return _current_hp<_max_hp;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Combat_system;

			float _auto_heal_max=0;
			float _auto_heal=0;

			float _max_hp = 0;
			float _current_hp = 0;

			float _damage=0, _heal=0;
	};

}
}
}
