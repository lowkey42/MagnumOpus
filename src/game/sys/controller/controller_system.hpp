/**************************************************************************\
 * Controlls the actions of all controllables                             *
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

#include "../../../core/input_manager.hpp"
#include "../../../core/asset/asset_manager.hpp"
#include "../../../core/units.hpp"

#include "mapping.hpp"
#include "controller.hpp"
#include "controllable_comp.hpp"
#include "input_controller.hpp"

namespace mo {
namespace sys {
namespace physics{class Transform_system;}

namespace controller {

	class Controller_manager {
		public:
			Controller_manager(asset::Asset_manager& assets, Input_manager& input);

			void update(Time dt);

			auto& main_controller(){return *_main_controller.get();}
			auto  gamepad(std::size_t idx, bool activate=false) -> util::maybe<Controller&>;

			bool player_ready()const noexcept {return _ready_gamepad_controller.size();}

			void screen_to_world_coords(std::function<glm::vec2(glm::vec2)> func) {
				_screen_to_world_coords = func;
			}

			util::signal_source<Controller_added_event> join_events;
			util::signal_source<Controller_removed_event> unjoin_events;
			util::signal_source<Quit_event> quit_events;

		private:
			void _join(Controller_added_event e);
			void _unjoin(Controller_removed_event e);

			void _add_gamepad(SDL_GameController* gp);
			void _remove_gamepad(SDL_GameController* gp);

		private:
			std::function<glm::vec2(glm::vec2)> _screen_to_world_coords;
			Mapping_ptr _mapping;

			util::slot<Controller_added_event> _on_joined;
			util::slot<Controller_removed_event> _on_unjoined;

			util::slot<SDL_GameController*> _on_added;
			util::slot<SDL_GameController*> _on_removed;

			std::unique_ptr<Controller> _main_controller;
			std::unique_ptr<Keyboard_controller> _keyboard_controller;
			std::vector<std::unique_ptr<Gamepad_controller>> _active_gamepad_controller;
			std::vector<std::unique_ptr<Gamepad_controller>> _ready_gamepad_controller;
	};


	class Controller_system {
		public:
			Controller_system(ecs::Entity_manager& entity_manager);

			void update(Time dt);

		private:
			void _on_comp_event(ecs::Component_event e);

			Controllable_comp::Pool& _controllables;
			util::slot<ecs::Component_event> _on_comp_events;
		};

}
}
}

