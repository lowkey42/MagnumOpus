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

namespace mo {
namespace sys {
namespace physics {

	class Transform_system;

	class Transform_comp : public ecs::Component<Transform_comp> {
		public:
			static constexpr const char* name() {return "Transform";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Transform_comp(ecs::Entity& owner, Distance x=Distance(0),
						   Distance y=Distance(0), Angle rotation=Angle(0))noexcept
			  : Component(owner), _position(x, y), _rotation(rotation), _max_rotation_speed(0) {}

			auto position()const noexcept {return _position;}
			void position(Position pos)noexcept {_position=pos; _dirty=true;}
			auto rotation()const noexcept {return _rotation;}
			void rotation(Angle a)noexcept {if(!_rotation_fixed) _rotation = a;}
			void rotate(Angle offset, Time dt) noexcept{
				auto max_rot = _max_rotation_speed*_max_rotation_speed_factor*dt;

				if(abs(offset)>max_rot)
					offset = sign(offset).value() * max_rot;

				rotation(rotation() + offset);
			}

			void set_max_rot_factor(float f) {_max_rotation_speed_factor = f;}

			auto layer()const noexcept {return _layer;}
			void layer(float layer)noexcept {
				INVARIANT(layer>=0 && layer<=1,"layer out of bounds!");
				_layer=layer;
			}

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Transform_system;

			Position _position;
			float _layer = 0.5f;
			Angle _rotation;
			bool _rotation_fixed = false;
			Angle_per_time _max_rotation_speed;
			float _max_rotation_speed_factor = 1.f;
			int32_t _cell_idx = -1;
			bool _dirty = true;
	};

}
}
}
