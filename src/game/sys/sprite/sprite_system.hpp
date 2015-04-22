/**************************************************************************\
 *	sprite_system.hpp							                          *
 *                                               ___                      *
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

namespace game{
namespace sys{
namespace sprite{

	class Sprite_system{

		public:

			// Constructors
			Sprite_system(core::ecs::Entity_manager& entity_manager, game::sys::physics::Transform_system& ts,
			              core::asset::Asset_manager& asset_manager) noexcept;

			// Methods
			void draw(const core::renderer::Camera& camera) noexcept;
			void update(core::Time dt) noexcept;

		private:

			sys::physics::Transform_system& _transform;
			core::renderer::Sprite_batch _sprite_batch;


	};

}
}
}
