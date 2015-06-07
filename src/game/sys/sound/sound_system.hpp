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
#include <core/audio/sound.hpp>
#include <core/audio/sound_ctx.hpp>
#include <core/renderer/camera.hpp>

#include "sound_comp.hpp"
#include "../state/state_system.hpp"

namespace mo{
namespace sys{
namespace sound{

	class Sound_system{

		public:

		// Constructors
		Sound_system(ecs::Entity_manager& entity_manager, physics::Transform_system& ts,
					 audio::Sound_ctx& snd_ctx, asset::Asset_manager& asset_manager,
					 state::State_system& state_system) noexcept;

		// Methods
		void play_sounds(const renderer::Camera& camera) noexcept;
		void update(Time dt) noexcept;


		private:

			void _on_state_change(ecs::Entity& e, state::State_data& data);

			physics::Transform_system& _transform;
			audio::Sound_ctx& _sound_ctx;
			sound::Sound_comp::Pool& _sounds;
			util::slot<ecs::Entity&, state::State_data&> _state_change_slot;

	};

}
}
}
