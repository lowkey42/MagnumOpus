/**************************************************************************\
 * ai master mind                                                         *
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

#include "simple_ai_comp.hpp"

namespace mo {
	class Game_engine;

namespace sys {
	namespace physics{ class Transform_system; }

	namespace ai {

		class Ai_system {
			public:
				Ai_system(ecs::Entity_manager& entity_manager, Game_engine& engine, physics::Transform_system& transform_system);

				void update(Time dt);

			private:
				Game_engine& _engine;
				Simple_ai_comp::Pool& _simples;
				physics::Transform_system& _transform_system;
		};

	}
}
}
