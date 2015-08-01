/**************************************************************************\
 * types of available effects and listeners                               *
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

#include <core/utils/events.hpp>

namespace mo {
namespace ecs{class Entity;}

	enum class Effect_type : uint16_t {
		none,

		element_fire,
		element_frost,
		element_water,
		element_stone,
		element_gas,
		element_lightning,

		health,


		flame_thrower,
		poison_thrower,
		frost_thrower,
		water_thrower,
		wind_thrower,

		steam,

		burning,
		poisoned,
		frozen,
		confused,

		explosion_fire,
		explosion_poison,
		explosion_ice,
		explosion_stone,
	};
	constexpr auto effect_type_count = static_cast<uint16_t>(Effect_type::flame_thrower)+1;

	using Effect_slot   = util::slot<ecs::Entity&,Effect_type>;
	using Effect_source = util::signal_source<ecs::Entity&,Effect_type>;

}

namespace std {
	template <> struct hash<mo::Effect_type> {
		size_t operator()(mo::Effect_type e)const noexcept {
			return static_cast<size_t>(e);
		}
	};
}


#ifdef MO_BUILD_SERIALIZER
#include <sf2/sf2.hpp>

namespace mo {
	sf2_enumDef(Effect_type,
		sf2_value(none),
		sf2_value(element_fire),
		sf2_value(element_fire),
		sf2_value(element_frost),
		sf2_value(element_water),
		sf2_value(element_stone),
		sf2_value(element_gas),
		sf2_value(element_lightning),

		sf2_value(health),

		sf2_value(flame_thrower),
		sf2_value(poison_thrower),
		sf2_value(frost_thrower),
		sf2_value(water_thrower),
		sf2_value(wind_thrower),

		sf2_value(steam),

		sf2_value(burning),
		sf2_value(poisoned),
		sf2_value(frozen),
		sf2_value(confused),

		sf2_value(explosion_fire),
		sf2_value(explosion_poison),
		sf2_value(explosion_ice),
		sf2_value(explosion_stone)
	)
}
#endif
