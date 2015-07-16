/**************************************************************************\
 * manages collection of collactable                                      *
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

#include "../../level/elements.hpp"
#include "../combat/comp/weapon_comp.hpp"

namespace mo {
	namespace asset {class Asset_manager;}
	namespace renderer {class Particle_renderer;}

namespace sys {
namespace item {

	class Element_system : public combat::Weapon_modifier {
		public:
			Element_system(asset::Asset_manager& assets,
			               ecs::Entity_manager& entity_manager,
			               combat::Weapon_modifier_collection& collection);
			~Element_system();

			void process(ecs::Entity& e, combat::Weapon&) override;
			void on_attack(ecs::Entity& e, const combat::Weapon&) override;


			struct Config;

		private:
			std::shared_ptr<const Config> _config;
	};

}
}
}
