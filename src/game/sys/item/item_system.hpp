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
#include <core/audio/audio_ctx.hpp>
#include <core/audio/sound.hpp>

#include "../physics/transform_system.hpp"
#include "../physics/physics_system.hpp"

#include "../state/state_system.hpp"

#include "collector_comp.hpp"

namespace mo {
	namespace asset {class Asset_manager;}
	namespace renderer {class Particle_renderer;}

namespace sys {
namespace item {

	struct Droprate_conf;

	class Item_system {
		public:
			Item_system(asset::Asset_manager& assets,
			            audio::Audio_ctx& audio,
			            ecs::Entity_manager& entity_manager,
			            physics::Physics_system& physics_system,
			            physics::Transform_system& transform,
			            state::State_system& state_system,
			            renderer::Particle_renderer& particles);

			void update(Time realtime_dt, Time dt);

			void up_score_multiplicator();
			auto score_multiplicator()const noexcept {return _score_multiplicator;}

			auto bullet_time_active()const noexcept {return _bullet_time_left>Time(0);}

		private:
			void _on_collision(physics::Manifold& m);
			void _drop_loot(ecs::Entity&, sys::state::State_data&);

			asset::Asset_manager&          _assets;
			audio::Audio_ctx&              _audio;
			ecs::Entity_manager&           _em;
			physics::Transform_system&     _ts;
			renderer::Particle_renderer&   _particles;
			Collector_comp::Pool&          _collectors;
			util::slot<physics::Manifold&> _collision_slot;
			util::slot<ecs::Entity&, sys::state::State_data&> _drop_loot_slot;
			std::shared_ptr<const Droprate_conf> _droprates;

			audio::Sound_ptr _pickup_sound_coin;
			audio::Sound_ptr _pickup_sound_health;
			audio::Sound_ptr _pickup_sound_other;

			float _score_multiplicator = 1.f;
			Time _score_mult_ttl = Time(0);

			Time _bullet_time_left = Time(0);
	};
}
}
}
