/**************************************************************************\
 * marks entities with ingame health-distplay                             *
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
#include <core/renderer/texture.hpp>

#include <array>

namespace mo {
namespace sys {
namespace ui {

	class Ui_minimal_comp : public ecs::Component<Ui_minimal_comp> {
		public:
			static constexpr const char* name() {return "ui_min";}

			Ui_minimal_comp(ecs::Entity& owner) noexcept
				: Component(owner) {
			}

			friend class Ui_system;
	};

}
}
}
