/**************************************************************************\
 * Stores coins                                                           *
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
namespace sys {
namespace combat {

	class Score_comp : public ecs::Component<Score_comp> {
		public:
			static constexpr const char* name() {return "Score";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Score_comp(ecs::Entity& owner, int value=0) noexcept
				: Component(owner), _value(value) {}

			void value(int value)noexcept {_value=value;}
			auto value()const noexcept {return _value;}

			void add(int mod)noexcept {_value+=mod;}

		private:
			friend class Collectable_subsystem;
			friend class Reaper_subsystem;

			int _value;
	};

}
}
}
