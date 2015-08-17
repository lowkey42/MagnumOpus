/**************************************************************************\
 * Manages the highscore list (load, store, add)                          *
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

#include <string>
#include <vector>

namespace mo {
	namespace asset{class Asset_manager;}


	struct Score {
		std::string name;
		int32_t     score;
		int32_t     level;
		uint64_t    seed;
	};

	extern void add_score(asset::Asset_manager& assets, Score score);

	extern auto list_scores(asset::Asset_manager& assets) -> std::vector<Score>;

	extern auto print_scores(std::vector<Score> scores) -> std::string;

}
