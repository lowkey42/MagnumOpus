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
#include <core/renderer/primitives.hpp>

#include "level/level.hpp"
#include "level/tilemap.hpp"

#include "sys/physics/transform_system.hpp"
#include "sys/physics/physics_system.hpp"
#include "sys/sprite/sprite_system.hpp"
#include "sys/controller/controller_system.hpp"
#include "sys/cam/camera_system.hpp"
#include "sys/ai/ai_system.hpp"
#include "sys/combat/combat_system.hpp"
#include "sys/state/state_system.hpp"


namespace mo {
	namespace renderer{ class Camera; }

	struct Profile_data {
		std::string name;
		uint64_t seed;
		int difficulty;
		int depth;

	};

	struct Game_state {
		Game_engine& engine;
		Profile_data profile;

		level::Level level;
		ecs::Entity_manager em;
		level::Tilemap tilemap;

		sys::physics::Transform_system transform;
		sys::cam::Camera_system camera;
		sys::physics::Physics_system physics;
		sys::state::State_system state;
		sys::controller::Controller_system controller;
		sys::ai::Ai_system ai;
		sys::combat::Combat_system combat;
		sys::sprite::Sprite_system spritesys;

		ecs::Entity_ptr main_player;
		std::vector<ecs::Entity_ptr> sec_players;

		renderer::Ray_renderer ray_renderer;

		Game_state(Game_engine& engine,
		           std::string profile,
		           std::vector<ecs::ETO> players,
		           util::maybe<int> depth);

		void update(Time dt);
		auto draw() -> util::cvector_range<sys::cam::VScreen>;

		auto add_player(sys::controller::Controller& controller, Position pos,
		                ecs::Entity_ptr e=ecs::Entity_ptr()) -> ecs::Entity_ptr;

		void delete_savegame();
	};

}
