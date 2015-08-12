/**************************************************************************\
 * Interface for controllable objects & controllers                       *
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

#include <glm/glm.hpp>

namespace mo {
namespace ecs {
	class Entity;
}
namespace sys {
namespace controller {

	struct Controllable_interface {
		virtual ~Controllable_interface()noexcept = default;
		virtual auto entity()noexcept -> ecs::Entity& = 0;
		virtual void move(glm::vec2 direction) = 0;
		virtual void look_at(glm::vec2 position)=0;
		virtual void look_in_dir(glm::vec2 direction)=0;
		virtual void attack() = 0;
		virtual void use() = 0;
		virtual void take() = 0;
		virtual void switch_weapon(uint32_t weapon_id) = 0;
	};

	struct Controller {
		virtual ~Controller()noexcept = default;
		virtual void operator()(Controllable_interface&) = 0;
		virtual void feedback(float force) {}
		virtual void request_unjoin() {}
	};


	struct Controller_removed_event {
		Controller& controller;
	};

	struct Controller_added_event {
		Controller& controller;
	};

	struct Quit_event {};

}
}
}
