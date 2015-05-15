/**************************************************************************\
 * initialization & event handling of input devices                       *
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

#include "utils/events.hpp"

#include <glm/vec2.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

namespace mo {

	class Input_manager {
		public:
			Input_manager();
			Input_manager(const Input_manager&) = delete;
			Input_manager(Input_manager&&) = delete;
			~Input_manager()noexcept;

			void update(float dt);
			void handle_event(SDL_Event& event);

		public: // event-sources
			util::signal_source<SDL_KeyboardEvent> keyboard_events;
			util::signal_source<SDL_MouseMotionEvent> mouse_events;
			util::signal_source<SDL_MouseButtonEvent> button_events;
			util::signal_source<SDL_GameController*> gamepad_added_events;
			util::signal_source<SDL_GameController*> gamepad_removed_events;

		private:
			void _add_gamepad(int joystick_id);
			void _remove_gamepad(int instance_id);

		private:

			std::vector<SDL_GameController*> _gamepads;
	};

}
