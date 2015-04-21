/**************************************************************************\
 * procedurally generates a level                                         *
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

#include "level.hpp"

namespace core {
namespace asset {
	class Asset_manager;
}
}

namespace game {
namespace level {

	/**
	 * @brief procedurally generates a level
	 * @param seed The seed used for the current playthrough
	 * @param depth The depth of the level (higher number is deeper => later in game)
	 * @param difficulty The base difficulty of the generated level (MIN_INT=treasure, MAX_INT="fun")
	 * @return the generated level
	 */
	extern Level generate_level(core::asset::Asset_manager& assets, uint64_t seed,
	                            int depth, int difficulty=0);

}
}
