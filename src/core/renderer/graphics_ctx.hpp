/**************************************************************************\
 * Window & OpenGL-Context creation + management                          *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *  Based on code of GDW-SS2014 project by Stefan Bodenschatz             *
 *  which was distributed under the MIT license.                          *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <SDL2/SDL.h>
#include <glm/vec3.hpp>

namespace mo {
	namespace asset{
		class Asset_manager;
	}

namespace renderer {
	class Graphics_ctx {
		public:
			Graphics_ctx(const std::string& name, asset::Asset_manager& assets);
			~Graphics_ctx();

			void start_frame();
			void end_frame(float delta_time);
			void set_clear_color(float r, float g, float b);

			void reset_viewport()const noexcept;

			auto win_width()const noexcept{return _win_width;}
			auto win_height()const noexcept{return _win_height;}

			auto max_screenshake()const noexcept -> float;

			void resolution(int width, int height, float max_screenshake=-1);


		private:
			asset::Asset_manager& _assets;
			std::string _name;
			int _win_width, _win_height;
			bool _fullscreen;
			float _max_screenshake;

			std::unique_ptr<SDL_Window,void(*)(SDL_Window*)> _window;
			SDL_GLContext _gl_ctx;
			glm::vec3 _clear_color;

			float _frame_start_time = 0;
			float _delta_time_smoothed = 0;
			float _cpu_delta_time_smoothed = 0;
			float _time_since_last_FPS_output = 0;
	};

	struct Disable_depthtest {
		Disable_depthtest();
		~Disable_depthtest();
	};
	struct Disable_depthwrite {
		Disable_depthwrite();
		~Disable_depthwrite();
	};
}
}

