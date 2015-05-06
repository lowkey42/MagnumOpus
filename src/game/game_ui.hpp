/**************************************************************************\
 * Ingame-UI renderer                                                     *
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

#include <memory>

namespace mo {
	class Game_engine;
	namespace renderer {
		class Camera;
	}
	namespace ecs {
		class Entity;
	}

	class Game_ui {
		public:
			Game_ui(Game_engine& engine);
			~Game_ui()noexcept;

			void pre_draw();
			void draw(const renderer::Camera& cam, ecs::Entity& entity, int offset=1);

		private:
			struct PImpl;
			std::unique_ptr<PImpl> _impl;
	};

}
