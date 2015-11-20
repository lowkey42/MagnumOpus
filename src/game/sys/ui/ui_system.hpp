/**************************************************************************\
 * Draws the per-player ingame ui                                         *
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
#include <core/utils/events.hpp>

#include <core/renderer/graphics_ctx.hpp>
#include <core/renderer/shader.hpp>
#include <core/renderer/text.hpp>
#include <core/renderer/camera.hpp>
#include <core/renderer/primitives.hpp>

#include "../physics/transform_system.hpp"

#include "ui_comp.hpp"

namespace mo {
	class Game_engine;

namespace sys {
namespace ui {

	class Ui_system {
		public:
			Ui_system(Game_engine& e, ecs::Entity_manager& em, physics::Transform_system& transforms,
			          std::function<float()> score_multiplicator);

			void update(Time dt);
			void draw(const renderer::Camera& world_cam);

		private:
			Ui_comp::Pool& _ui_comps;

			renderer::Camera _cam;
			renderer::Shader_program _hud_shader;
			renderer::Shader_program _health_shader;
			renderer::Object _hud;
			renderer::Texture_ptr _hud_bg_tex;
			renderer::Texture_ptr _hud_fg_tex;
			renderer::Texture_ptr _hud_health_tex;
			renderer::Texture_ptr _hud_health_min_tex;

			renderer::Shader_program _score_shader;
			renderer::Font_ptr    _score_font;
			renderer::Font_ptr     _score_mult_font;
			renderer::Text_dynamic _score_text;
			renderer::Text_dynamic _score_mult_text;

			renderer::Textured_box _join_msg;

			renderer::Bubble_renderer _bubble_renderer;
			asset::Asset_manager& _assets;
			physics::Transform_system& _transforms;

			Time _time_acc = Time{0};

			std::function<bool()> _player_ready;
			bool _last_player_ready = false;
			Time _join_msg_fade = Time{0};

			std::function<float()> _score_multiplicator;
	};

}
}
}
