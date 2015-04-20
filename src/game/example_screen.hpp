/**************************************************************************\
 * A simple example/ test screen                                          *
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

#include "../core/engine.hpp"
#include "../core/renderer/texture.hpp"
#include "../core/renderer/shader.hpp"
#include "../core/renderer/vertex_object.hpp"
#include "../core/renderer/text.hpp"

namespace game {

	class Example_screen : public core::Screen {
		public:
			Example_screen(core::Engine& engine);
			~Example_screen()noexcept = default;

		protected:
			void _update(float delta_time)override;
			void _draw(float delta_time)override;

			auto _prev_screen_policy()const noexcept -> core::Prev_screen_policy override {
				return core::Prev_screen_policy::discard;
			}

		private:
			core::renderer::Shader_program _shader;
			core::renderer::Object _object;

			core::renderer::Shader_program _text_shader;
			core::renderer::Font_ptr _font;
	};

}
