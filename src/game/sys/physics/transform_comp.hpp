/**************************************************************************\
 * provides the transform-data (position, rotation)                       *
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

#include "../../../core/ecs/ecs.hpp"
#include "../../../core/units.hpp"

namespace game {
namespace sys {
namespace physics {

	class Transform_system;

	class Transform_comp : public core::ecs::Component<Transform_comp> {
		public:
			static constexpr const char* name() {return "Transform";}
			void load(core::ecs::Entity_state&)override;
			void store(core::ecs::Entity_state&)override;

			Transform_comp(core::ecs::Entity& owner, core::Distance x=core::Distance(0),
			               core::Distance y=core::Distance(0), core::Angle rotation=core::Angle(0))noexcept
			  : Component(owner), _position(x, y), _rotation(rotation) {}

			auto position()const noexcept {return _position;}
			void position(core::Position pos)noexcept {_position=pos; _dirty=true;}
			auto rotation()const noexcept {return _rotation;}
			void rotation(core::Angle a)noexcept {_rotation = a;}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Transform_system;

			core::Position _position;
			core::Angle _rotation;
			int32_t _cell_idx = -1;
			bool _dirty = true;
	};

}
}
}
