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

namespace game {
namespace sys {
namespace cam {

	class Camera_target_comp : public core::ecs::Component<Camera_target_comp> {
		public:
			static constexpr const char* name() {return "Camera_target";}
			void load(core::ecs::Entity_state&)override;
			void store(core::ecs::Entity_state&)override;

			Camera_target_comp(core::ecs::Entity& owner, core::Mass mass=core::Mass(0.1), float damping=0.5, float stiffness=0.85)
			    : Component(owner), _mass(mass), _damping(damping), _stiffness(stiffness) {}

			void force_position(core::Position pos)noexcept {_cam_pos=pos;}

			auto cam_position()const noexcept {return _cam_pos;}

			void chase(core::Position target, core::Time dt);

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			bool _unset=true;
			core::Mass _mass;
			float _damping;
			float _stiffness;
			core::Velocity _velocity;

			core::Position _cam_pos;
	};

}
}
}
