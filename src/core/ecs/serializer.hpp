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
#include "../asset/asset_manager.hpp"

#include <sf2/sf2.hpp>


namespace mo {
namespace asset {
	class AID;
	class Asset_manager;
}

namespace ecs {

	struct EcsSerializer : public sf2::JsonSerializer {
		EcsSerializer(std::ostream& stream, Entity_manager& m,
		              asset::Asset_manager& assets)
			: sf2::JsonSerializer(stream),
			  manager(m), assets(assets) {
		}

		Entity_manager& manager;
		asset::Asset_manager& assets;
	};
	struct EcsDeserializer : public sf2::JsonDeserializer {
		EcsDeserializer(const std::string& source_name,
		                std::istream& stream, Entity_manager& m,
		                asset::Asset_manager& assets);

		Entity_manager& manager;
		asset::Asset_manager& assets;
	};

	extern void load(sf2::JsonDeserializer& s, Entity& e);
	extern void save(sf2::JsonSerializer& s, const Entity& e);
	extern void load(sf2::JsonDeserializer& s, Entity_ptr& e);


	class Blueprint;

	extern void apply_blueprint(asset::Asset_manager&, Entity& e,
	                            asset::AID blueprint);
}
}
