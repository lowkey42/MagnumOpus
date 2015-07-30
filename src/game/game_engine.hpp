/**************************************************************************\
 * Game-specific engine                                                   *
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

#include <core/engine.hpp>
#include <core/configuration.hpp>

#include <game/sys/controller/controller_system.hpp>

namespace mo {

	class Game_engine : public Engine {
		public:
			Game_engine(const std::string& title, int argc, char** argv, char** env,
			            bool start_game=true);
			~Game_engine();

			/// re-define enter_screen to inject Game_engine instead of Engine into screens
			using Engine::enter_screen;

			template<class T, typename ...Args>
			auto enter_screen(Args&&... args) -> T& {
				return static_cast<T&>(enter_screen(std::make_unique<T>(*this, std::forward<Args>(args)...)));
			}

			auto controllers()noexcept -> sys::controller::Controller_manager & {
				return _controllers;
			}
			auto controllers()const noexcept -> const sys::controller::Controller_manager & {
				return _controllers;
			}

		protected:
			void _on_frame(float dt) override {
				_controllers.update(Time(dt));
			}
			void _on_quit(sys::controller::Quit_event) {
				exit();
			}
			auto _on_reload() -> std::tuple<bool, std::string> override;

		private:
			sys::controller::Controller_manager _controllers;
			util::slot<sys::controller::Quit_event> _on_quit_slot;
	};

}
