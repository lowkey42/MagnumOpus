/**************************************************************************\
 * The screen that is shown after the player died to enter his name       *
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
#include "highscore.hpp"

namespace mo {

	class Gameover_screen : public Screen, public Ui_container {
		public:
			Gameover_screen(Game_engine& game_engine, Score score);
			~Gameover_screen()noexcept = default;

		protected:
			void _update(float delta_time)override;
			void _draw(float delta_time)override;

			void _on_enter(util::maybe<Screen&> prev) override;
			void _on_leave(util::maybe<Screen&> next) override;

			auto _prev_screen_policy()const noexcept -> Prev_screen_policy override {
				return Prev_screen_policy::discard;
			}

		private:
			renderer::Texture_ptr _background;
			renderer::Texture_ptr _circle;
			bool _highscore_reached;
			renderer::Text_dynamic _message;
			renderer::Text_dynamic _hint;
			renderer::Text_dynamic _new_highscore;
			Score _score;
			bool _quit=false;

			Time _time_acc = Time{0};
	};

}
