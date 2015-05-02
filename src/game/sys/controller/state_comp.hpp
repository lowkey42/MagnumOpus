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
namespace controller {

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

	// TODO: events for sounds

	class State_comp : public ecs::Component<State_comp> {
		public:
			static constexpr const char* name() {return "State";}

			State_comp(ecs::Entity& owner, Entity_state state = Entity_state::idle) noexcept
				: Component(owner), _current_state(state), _time_left(_required_time_for(state)) {}

			void state(Entity_state s)noexcept;
			auto state()const noexcept {return _current_state;}

			void update(Time dt)noexcept;

		private:
			Entity_state _current_state;
			Entity_state _last_state;
			Time _time_left;

			Time _required_time_for(Entity_state state)const noexcept;
	};

}
}
}
