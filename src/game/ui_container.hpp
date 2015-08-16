/**************************************************************************\
 * Container that manages the ui widgets it contains (draw, events, ...)  *
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

#include <core/gui/widget.hpp>


namespace mo {

	class Ui_container : public gui::Ui_ctx {
		public:
			Ui_container(Game_engine& game_engine, glm::vec2 screen_size,
			             glm::vec2 position,
			             glm::vec2 size, gui::Layout layout);
			~Ui_container();

			auto create_texture(const std::string& type) -> gui::Ui_texture override;
			auto create_text(const std::string& str) -> gui::Ui_text override;

			void draw(gui::Ui_texture& tex, glm::vec2 center, bool highlight,
				              glm::vec4 clip) override;

			void draw(gui::Ui_text& tex, glm::vec2 center, bool highlight) override;

			void play_activate() override;
			void play_focus() override;

			void draw_ui();
			void update_ui();

			void enable();
			void disable();

		protected:
			struct ui_controller;

			Game_engine& _game_engine;
			renderer::Camera _camera;

			renderer::Sprite_renderer _sprite_renderer;
			renderer::Text_renderer _text_renderer;

			renderer::Font_ptr _font;

			gui::Widget_container _root;

			int _updates_to_skip;

			std::unique_ptr<ui_controller> _controller;
	};

}
