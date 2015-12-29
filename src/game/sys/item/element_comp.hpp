/**************************************************************************\
 * Contains elemental effects & available weapon-elements                 *
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

#include <array>

#include "../../level/elements.hpp"

namespace mo {
namespace sys {
namespace item {

	constexpr auto element_slots = 4u;

	struct Element_slot {
		level::Element element = level::Element::neutral;
		float fill = 0.f;
		bool active = false;
	};

	class Element_comp : public ecs::Component<Element_comp> {
		public:
			static constexpr const char* name() {return "Element";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Element_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			void self_effect(level::Elements e)noexcept {_self_effects=e;}
			auto self_effect()const noexcept {return _self_effects;}
			auto self_effect()noexcept -> auto& {return _self_effects;}

			void slot(std::size_t i, const Element_slot& s)noexcept {_slots.at(i)=s;}
			auto slot(std::size_t i)const noexcept -> auto& {return _slots.at(i);}
			auto slot(std::size_t i)noexcept -> auto& {return _slots.at(i);}

			auto flip_slot     (std::size_t i               )noexcept -> bool;
			auto mod_slot_fill (std::size_t i, float mod    )noexcept -> bool;
			auto mod_slots_fill(float mod                   )noexcept -> bool;
			auto add_slot      (level::Element e, float fill)noexcept -> bool;

			struct Persisted_state;
			friend struct Persisted_state;
			friend class Element_system;
		private:
			level::Elements _self_effects;
			std::array<Element_slot, element_slots> _slots;

	};

}
}
}
