/**************************************************************************\
 * universal health-care & murder system                                  *
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
#include <core/units.hpp>

#include "weapon_comp.hpp"
#include "health_comp.hpp"

namespace mo {
	namespace asset {class Asset_manager;}

namespace sys {
namespace combat {

	class Combat_system {
		public:
			Combat_system(ecs::Entity_manager& entity_manager, asset::Asset_manager& assets);

			void update(Time dt);

		private:
			void _health_care(Time dt);
			void _shoot_something(Time dt);

			ecs::Entity_manager&  _em;
			asset::Asset_manager& _assets;
			Weapon_comp::Pool& _weapons;
			Health_comp::Pool& _healths;
	};

}
}
}
