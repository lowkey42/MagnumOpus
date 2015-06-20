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
		element
	};

	class Item_comp : public ecs::Component<Item_comp> {
		public:
			static constexpr const char* name() {return "Item";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Item_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}


			struct Persisted_state;
			friend struct Persisted_state;
			friend class Item_system;
		private:

			Item_target    _target;
			level::Element _element;
			float          _mod;
			bool           _joinable;
			bool           _collected = false;
	};

}
}
}
