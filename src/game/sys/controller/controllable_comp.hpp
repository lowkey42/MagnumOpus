/**************************************************************************\
 * Entity is controllable by external inputs                              *
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

#include <vector>
#include <glm/glm.hpp>
#include <core/ecs/ecs.hpp>

#include "controller.hpp"

namespace game {
namespace sys {
namespace controller {

	class Controllable_comp : public core::ecs::Component<Controllable_comp> {
		public:
			static constexpr const char* name() {return "Controllable";}
			// static void load(sf2::io::CharSource& cs, ecs::ComponentBase& comp){}
			// static void store(sf2::io::CharSink& cs, const ecs::ComponentBase& comp){}

			Controllable_comp(core::ecs::Entity& owner, Controller* controller=nullptr) noexcept
				: Component(owner), controller(controller) {}


			Controller* controller;
	};

}
}
}
