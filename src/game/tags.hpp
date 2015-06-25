/**************************************************************************\
 * marks a "player"                                                       *
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

namespace mo {

	class Player_tag_comp : public ecs::Component<Player_tag_comp> {
		public:
			static constexpr const char* name() {return "Player_tag";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Player_tag_comp(ecs::Entity& owner, uint8_t id=0): Component(owner), _id(id) {}

			auto id()const noexcept{return _id;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			uint8_t _id;
	};

}
