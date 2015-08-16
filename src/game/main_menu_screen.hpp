/**************************************************************************\
 * The main menu                                                          *
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

#include "game_engine.hpp"
#include "../core/engine.hpp"
#include <core/renderer/camera.hpp>

#include <core/renderer/texture.hpp>
#include <core/renderer/primitives.hpp>
#include <core/renderer/text.hpp>

#include "ui_container.hpp"

namespace mo {

	class Main_menu_screen : public Screen, public Ui_container {
		public:
			Main_menu_screen(Game_engine& game_engine, bool ingame=false);
			~Main_menu_screen()noexcept = default;

		protected:
			void _update(float delta_time)override;
			void _draw(float delta_time)override;

			void _on_enter(util::maybe<Screen&> prev) override;
			void _on_leave(util::maybe<Screen&> next) override;

			void _on_quit(sys::controller::Quit_event) {
				_quit = true;
			}

			auto _prev_screen_policy()const noexcept -> Prev_screen_policy override {
				return _ingame ? Prev_screen_policy::draw : Prev_screen_policy::discard;
			}

		private:
			const bool _ingame;
			const glm::vec2 _screen_size;

			bool _quit = false;
			util::slot<sys::controller::Quit_event> _on_quit_slot;

			renderer::Texture_ptr _background;
			renderer::Texture_ptr _circle;

			Time _time_acc = Time{0};
	};

}
