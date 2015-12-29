/**************************************************************************\
 * lasting effects like burning, poisoned, ...                            *
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

#include "../../../level/elements.hpp"
#include "../../../effects.hpp"

namespace mo {
namespace sys {
namespace combat {

	enum Damage_effect {
		none,
		burning,
		poisoned,
		wet,
		frozen,
		blinded
	};
	constexpr auto damge_effect_count = std::size_t(Damage_effect::blinded) +1;

	class Damage_effect_comp : public ecs::Component<Damage_effect_comp> {
		public:
			static constexpr const char* name() {return "DmgEffect";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Damage_effect_comp(ecs::Entity& owner,
			                   Damage_effect etype=Damage_effect::none) noexcept
				: Component(owner) {effect_type(etype);}

			void effect_type(Damage_effect type)noexcept {_next_type=type;}
			auto effect_type()const noexcept {return _type;}

			auto confusion()const noexcept {return _confusion;}

			friend class Combat_system;
		private:
			Damage_effect _type = Damage_effect::none;

			Damage_effect _next_type   = Damage_effect::none;
			Time          _time_left   = Time{0};
			float         _confusion   = 0;
	};

}
}
}

#ifdef MO_BUILD_SERIALIZER
#include <sf2/sf2.hpp>

namespace mo {
namespace sys {
namespace combat {

	sf2_enumDef(Damage_effect,
		none,
		burning,
		poisoned,
		wet,
		frozen,
		blinded
	)

}
}
}
#endif
