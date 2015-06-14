/**************************************************************************\
 * The screen in which the core-gameplay take place                       *
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

#include "game_engine.hpp"

#include "../core/units.hpp"
#include "../core/ecs/ecs.hpp"
#include "../core/ecs/serializer.hpp"
#include <core/renderer/particles.hpp>

#include "level/level.hpp"
#include "level/tilemap.hpp"

#include "sys/physics/transform_system.hpp"
#include "sys/physics/physics_system.hpp"
#include "sys/sprite/sprite_system.hpp"
#include "sys/sound/sound_system.hpp"
#include "sys/controller/controller_system.hpp"
#include "sys/cam/camera_system.hpp"
#include "sys/ai/ai_system.hpp"
#include "sys/combat/combat_system.hpp"
#include "sys/state/state_system.hpp"
#include "sys/ui/ui_system.hpp"

namespace mo {
	namespace renderer{ class Camera; }

	struct Profile_data {
		std::string name;
		uint64_t seed;
		int difficulty;
		int depth;

	};

	struct Saveable_state;
	using Saveable_state_ptr = asset::Ptr<Saveable_state>;

	struct Game_state {
		Game_engine& engine;
		Profile_data profile;

		level::Level level;
		ecs::Entity_manager em;
		level::Tilemap tilemap;

		sys::physics::Transform_system transform;

		renderer::Particle_renderer particle_renderer;

		sys::cam::Camera_system camera;
		sys::physics::Physics_system physics;
		sys::state::State_system state;
		sys::controller::Controller_system controller;
		sys::ai::Ai_system ai;
		sys::combat::Combat_system combat;
		sys::sprite::Sprite_system spritesys;
		sys::sound::Sound_system soundsys;
		sys::ui::Ui_system ui;

		ecs::Entity_ptr main_player;
		std::vector<ecs::Entity_ptr> sec_players;


		void update(Time dt);
		auto draw(Time dt) -> util::cvector_range<sys::cam::VScreen>;
		void draw_ui();

		auto add_player(sys::controller::Controller& controller, Position pos,
		                ecs::Entity_ptr e=ecs::Entity_ptr()) -> ecs::Entity_ptr;

		void delete_savegame();
		auto save() -> Saveable_state;


		static auto create(Game_engine& engine,
		                   std::string profile,
				           std::vector<ecs::ETO> players,
		                   util::maybe<int> depth) -> std::unique_ptr<Game_state>;
		static auto create_from_save(Game_engine& engine,
		                             const Saveable_state&) -> std::unique_ptr<Game_state>;

		private:
			Game_state(Game_engine& engine, int depth);
	};

	struct Saveable_state {
		Saveable_state(ecs::Entity_manager& em) : em(em), my_stream(util::nothing()) {}
		Saveable_state(asset::istream stream) : em(util::nothing()), my_stream(std::move(stream)) {}
		Saveable_state(Saveable_state&&)noexcept = default;

		auto operator=(Saveable_state&&)noexcept -> Saveable_state& = default;

		util::maybe<ecs::Entity_manager&> em;
		util::maybe<asset::istream> my_stream;
	};

	namespace asset {
		template<>
		struct Loader<Saveable_state> {
			using RT = std::shared_ptr<Saveable_state>;

			static RT load(istream in) throw(Loading_failed);

			static void store(ostream out, const Saveable_state& asset) throw(Loading_failed);
		};
	}
}
