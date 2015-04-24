/**************************************************************************\
 * manages cameras                                                        *
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

#include "../../../core/ecs/ecs.hpp"
#include "../../../core/units.hpp"
#include "../../../core/utils/template_utils.hpp"
#include "../../../core/renderer/camera.hpp"

#include "camera_target_comp.hpp"

class Game_engine;

namespace game {
namespace sys {
namespace cam {

	class Camera_system {
		public:
			Camera_system(core::ecs::Entity_manager& entity_manager, Game_engine& engine);

			void update(core::Time dt);

			auto cameras()const noexcept {return core::util::range(_cameras);}

		private:
			Game_engine& _engine;
			Camera_target_comp::Pool& _targets;
			std::vector<core::renderer::Camera> _cameras;
	};

}
}
}
