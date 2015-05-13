/**************************************************************************\
 * Reads & writes entites to disc                                         *
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
#include <unordered_map>

#include "ecs.hpp"

namespace mo {
	namespace asset {
		class AID;
		class Asset_manager;
	}

	namespace ecs {

		// entity transfer object
		using ETO = std::string;

		class Serializer {
			public:
				Serializer(Entity_manager& entityMgr, asset::Asset_manager& assetMgr);

				Entity_ptr apply(const asset::AID& blueprint, Entity_ptr target)const;
				void detach(Entity_ptr target)const;

				std::string write();
				void read(const std::string& c);

				void on_reload();

			private:
				Entity_manager& _entities;
				asset::Asset_manager& _assets;
		};

	}
}
