/**************************************************************************\
 *	sound_system.hpp	-   Sound Component Management System             *
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
#include <core/audio/audio_ctx.hpp>
#include <core/renderer/camera.hpp>

#include "sound_comp.hpp"

namespace mo{
namespace sys{
namespace sound{

	class Sound_system{

		public:

			// Constructors
			Sound_system(ecs::Entity_manager& entity_manager, physics::Transform_system& ts,
						 audio::Audio_ctx& audio_ctx) noexcept;

			// Methods
			void play_sounds(const renderer::Camera& camera) noexcept;


		private:
			physics::Transform_system& _transform;
			audio::Audio_ctx& _audio_ctx;
			sound::Sound_comp::Pool& _sounds;

	};

}
}
}
