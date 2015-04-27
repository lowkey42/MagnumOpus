/**************************************************************************\
 * provides movement and basic physical interactions                      *
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
#include <glm/gtx/norm.hpp>
#include "transform_comp.hpp"

#include "manifold.hpp"

#include "../../../core/ecs/ecs.hpp"
#include "../../../core/units.hpp"

namespace game {
namespace sys {
namespace physics {

	struct World_position;

	class Physics_comp : public core::ecs::Component<Physics_comp> {
		public:
			static constexpr const char* name() {return "Physics";}
			void load(core::ecs::Entity_state&)override;
			void store(core::ecs::Entity_state&)override;

			Physics_comp(core::ecs::Entity& owner,
						 core::Distance body_radius=core::Distance(1),
						 core::Mass mass=core::Mass(1), float restitution=1,
						 float friction=1, bool solid=true) noexcept;

			void accelerate_active(glm::vec2 dir)noexcept;
			void accelerate(core::Acceleration acc)noexcept;
			void impulse(core::Dir_force force)noexcept;
			void apply_force(core::Dir_force force)noexcept;
			void velocity(core::Velocity velocity)noexcept;

			void mass(core::Mass m) {
				_inv_mass = 1.f/m;
			}
			auto mass()const -> core::Mass {
				return 1.f/_inv_mass;
			}

			auto radius()const noexcept {return _body_radius;}
			void radius(core::Distance r)noexcept {_body_radius=r;}
			auto velocity()const noexcept {return _velocity;}
			auto acceleration()const noexcept {return _acceleration;}
			auto active()const noexcept {return _active;}


			struct Persisted_state;
		private:
			friend struct Persisted_state;
			friend class Physics_system;

			core::Distance _body_radius;
			core::Inv_mass _inv_mass;
			float _restitution;
			float _friction;
			core::Speed _max_active_velocity2 = core::Speed(0);
			core::Speed_per_time _active_acceleration = core::Speed_per_time(0);

			bool _solid;
			bool _active;

			core::Velocity _velocity;
			core::Acceleration _acceleration;
	};

}
}
}
