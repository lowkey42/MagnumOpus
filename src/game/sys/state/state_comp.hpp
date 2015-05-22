/**************************************************************************\
 * stores the current state of an entity (e.g. walking, idle, attacking)  *
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
namespace state {

	enum class Entity_state {
		idle,
		walking,
		attacking_melee,
		attacking_range,
		interacting,
		taking,
		change_weapon,
		damaged,
		healed,
		died,
		resurrected
	};
	struct State_data {
		Entity_state s = Entity_state::idle;
		float magnitude = 1.f;
		Time left = Time(0);

		void min_time(Time m)noexcept {
			left = max(left, m);
		}

		auto operator!=(const State_data& rhs)const noexcept {
			return s!=rhs.s || magnitude!=rhs.magnitude;
		}
	};

	class State_comp : public ecs::Component<State_comp> {
		public:
			static constexpr const char* name() {return "State";}

			State_comp(ecs::Entity& owner, Entity_state s = Entity_state::idle) noexcept
				: Component(owner) {
				state(s);
			}

			void state(Entity_state s, float magnitude = 1.f)noexcept;
			auto state()const noexcept {return _state_last;}

			auto update(Time dt)noexcept -> util::maybe<State_data&>;

		private:

			State_data _state_primary;
			State_data _state_last;
			State_data _state_background;
	};

}
}
}
