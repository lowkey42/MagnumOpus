/**************************************************************************\
 * Controls entities based on user-input                                  *
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

#include "../../../core/utils/events.hpp"

#include "controller.hpp"
#include "mapping.hpp"

namespace game {
namespace sys {
namespace controller {

	class Input_controller_base : public Controller, core::util::no_copy_move {
		public:
			Input_controller_base(Mapping_ptr mapping)
				: _mapping(mapping), _h_dir(0), _v_dir(0), _attack(false), _use(false), _weapon{0} {}

			void on_frame();

			static constexpr uint8_t weapon_count = 4;

		protected:
			void _on_command(Command cmd, bool active);

			Mapping_ptr _mapping;

			int8_t _h_dir;
			int8_t _v_dir;
			bool _attack;
			bool _use;
			int _weapon[weapon_count];
	};


	class Keyboard_controller : public Input_controller_base {
		public:
			Keyboard_controller(Mapping_ptr mapping, std::function<glm::vec2(glm::vec2)>& screen_to_world_coords,
							   core::util::signal_source<SDL_KeyboardEvent>& keys,
							   core::util::signal_source<SDL_MouseMotionEvent>& mouse,
							   core::util::signal_source<SDL_MouseButtonEvent>& button);

			void operator()(Controllable_interface&);

		private:
			void _on_key(SDL_KeyboardEvent event);
			void _on_mouse_moved(SDL_MouseMotionEvent event);
			void _on_button_clicked(SDL_MouseButtonEvent event);

		private:
			std::function<glm::vec2(glm::vec2)>& _screen_to_world_coords;
			core::util::slot<SDL_KeyboardEvent> _key_events;
			core::util::slot<SDL_MouseMotionEvent> _mouse_events;
			core::util::slot<SDL_MouseButtonEvent> _button_events;

			glm::vec2 _mouse_pos;
	};

	class Gamepad_controller : public Input_controller_base {
		public:
			Gamepad_controller(Mapping_ptr mapping, SDL_GameController* controller,
							  core::util::signal_source<Controller_added_event>& added_events,
							  core::util::signal_source<Controller_removed_event>& removed_events);
			~Gamepad_controller();

			void operator()(Controllable_interface&);

			void feedback(float force);

			void on_frame();

			void enter_or_leave();

			auto instance_id()const -> SDL_JoystickID;
			auto jId()const -> int;

		private:
			SDL_GameController* _controller;
			SDL_Haptic* _haptic;
			core::util::signal_source<Controller_added_event>& _added_events;
			core::util::signal_source<Controller_removed_event>& _removed_events;
			glm::vec2 _move;
			glm::vec2 _look;
			bool _active;

			bool _pressed[command_count];
	};

	class Combined_controller : public Controller {
		public:
			Combined_controller(Keyboard_controller& keyboard, std::vector<std::unique_ptr<Gamepad_controller>>& gamepads);

			void operator()(Controllable_interface&);
			void feedback(float force);

		private:
			Keyboard_controller& _keyboard;
			std::vector<std::unique_ptr<Gamepad_controller>>& _gamepads;
	};

}
}
}
