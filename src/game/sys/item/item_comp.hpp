/**************************************************************************\
 * Items such as coins, potions or elements                               *
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

#include "../../level/elements.hpp"

namespace mo {
namespace sys {
namespace item {

	enum Item_target {
		health,
		score,
		element,
		bullet_time
	};

	class Item_comp : public ecs::Component<Item_comp> {
		public:
			static constexpr const char* name() {return "Item";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Item_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}


			friend class Item_system;
		private:

			Item_target    _target = Item_target::score;
			level::Element _element = level::Element::neutral;
			float          _mod = 0.f;
			bool           _joinable = false;
			bool           _collected = false;
	};

}
}
}
