/**************************************************************************\
 * target for camera following                       *
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
namespace cam {

	class Camera_target_comp : public ecs::Component<Camera_target_comp> {
		public:
			static constexpr const char* name() {return "Camera_target";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Camera_target_comp(ecs::Entity& owner, Mass mass=Mass(0.1), float damping=0.5, float freq=3, float lazyness=2)
			    : Component(owner), _mass(mass), _damping(damping), _freq(freq), _lazyness(lazyness) {}

			void force_position(Position pos)noexcept {_cam_pos=pos;}

			auto cam_position()const noexcept {return _cam_pos;}

			void chase(Position target, Time dt);
			void reset();

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			friend class Camera_system;

			bool _unset=true;
			Time _sleeping = Time(0);
			Mass _mass;
			float _damping;
			float _freq;
			float _lazyness;
			Velocity _velocity;

			Position _cam_pos;

			// TODO[foe]: refactor
			Angle _last_rotation = Angle{0};
			Time _rotation_zoom_time_left = Time{0};
			Time _rotation_zoom_time=Time{1.f};
	};

}
}
}
