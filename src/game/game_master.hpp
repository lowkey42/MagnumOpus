/**************************************************************************\
 * Manages enemy spawn-points, global behaivor, etc.                      *
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

#include "../game_engine.hpp"
#include "level/level.hpp"
#include "level/level_generator.hpp"

#include "../core/utils/maybe.hpp"


namespace game {

	struct Game_state;

	struct Saved_game_state {
		uint64_t seed;
		int current_level;
		int current_difficulty;
		core::util::maybe<level::Level> stored_level;
		// TODO: add ECS-save
	};


	class Game_master {
		public:
			Game_master(Game_engine& engine, const Saved_game_state& state);

			void update();

			auto& level()noexcept {return _level;}
			auto& level()const noexcept {return _level;}

		private:
			level::Level _level;
	};

}
