/**************************************************************************\
 *	Sprite Component Management System                                    *
 *                                                ___                     *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include "../../sys/physics/transform_system.hpp"
#include <core/renderer/sprite_batch.hpp>
#include <core/renderer/camera.hpp>

#include "sprite_comp.hpp"
#include "particle_emiter_comp.hpp"
#include "../state/state_system.hpp"

namespace mo{
namespace sys{
namespace graphic{

	class Graphic_system {
		public:
			Graphic_system(ecs::Entity_manager& entity_manager, physics::Transform_system& ts,
						  asset::Asset_manager& asset_manager, state::State_system& state_system) noexcept;

			void draw(const renderer::Camera& camera) noexcept;
			void update(Time dt) noexcept;

		private:
			void _on_state_change(ecs::Entity& e, state::State_data& data);

			physics::Transform_system& _transform;
			renderer::Sprite_batch _sprite_batch;
			Sprite_comp::Pool& _sprites;
			util::slot<ecs::Entity&, state::State_data&> _state_change_slot;
	};

}
}
}
