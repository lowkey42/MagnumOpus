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

#include "../physics/transform_system.hpp"
#include "../physics/physics_system.hpp"

#include "collector_comp.hpp"

namespace mo {
	namespace asset {class Asset_manager;}
	namespace renderer {class Particle_renderer;}

namespace sys {
namespace combat {

	class Collectable_subsystem {
		public:
			Collectable_subsystem(asset::Asset_manager& assets,
			                      ecs::Entity_manager& entity_manager,
			                      physics::Transform_system& transform,
			                      renderer::Particle_renderer& particles);

			void update(Time dt);
			void _on_collision(physics::Manifold& m);

		private:
			asset::Asset_manager& _assets;
			ecs::Entity_manager& _em;
			physics::Transform_system& _ts;
			renderer::Particle_renderer& _particles;
			Collector_comp::Pool& _collectors;

	};
}
}
}
