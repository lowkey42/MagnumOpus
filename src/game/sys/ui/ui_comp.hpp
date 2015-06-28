/**************************************************************************\
 * marks entities with ingame ui                                          *
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

	class Ui_comp : public ecs::Component<Ui_comp> {
		public:
			static constexpr const char* name() {return "ui";}

			Ui_comp(ecs::Entity& owner) noexcept
				: Component(owner) {
			}

			friend class Ui_system;
		private:
			struct Element_state {
				float activity = 0;
				float fill = 0.f;
				renderer::Texture_ptr texture;
			};

			bool _initialized = false;

			std::array<Element_state,4> _elements;
			float _health = 0;
			int _score = 0;
			glm::vec3 _offset;
			glm::vec3 _target_offset;

			glm::mat4 _mvp;
	};

}
}
}
