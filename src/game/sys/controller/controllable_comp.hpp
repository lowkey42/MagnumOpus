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

namespace mo {
namespace sys {
namespace controller {

	class Controllable_comp : public ecs::Component<Controllable_comp> {
		public:
			static constexpr const char* name() {return "Controllable";}

			Controllable_comp(ecs::Entity& owner) noexcept
				: Component(owner) {}

			Controllable_comp(ecs::Entity& owner, Controller* controller) noexcept
				: Component(owner), _controller(controller) {}

			template<class T, typename = std::enable_if_t<ecs::is_component<T>::value>>
			Controllable_comp(ecs::Entity& owner, T*) noexcept
				: Component(owner), _controller_component(T::type()) {}

			Controllable_comp(ecs::Entity& owner, ecs::Component_type controller) noexcept
				: Component(owner), _controller_component(controller) {}


			void set(Controller& controller) {
				_controller = &controller;
				_controller_component = 0;
			}
			template<class T, typename = std::enable_if_t<ecs::is_component<T>::value>>
			void set(T&) {
				set(T::type());
			}


			void set(ecs::Component_type controller) {
				_controller = nullptr;
				_controller_component = controller;
			}

			bool is(Controller& controller) {
				return _controller==&controller;
			}
			template<class T, typename = std::enable_if_t<ecs::is_component<T>::value>>
			bool is(T& controller) {
				return _controller_component==T::type();
			}

			void feedback(float force) {
				if(_controller)
					_controller->feedback(force);
			}

		private:
			friend class Controller_system;

			Controller* _controller = nullptr;
			ecs::Component_type _controller_component = 0;
	};

}
}
}
