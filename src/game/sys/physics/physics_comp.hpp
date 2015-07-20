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

namespace mo {
namespace sys {
namespace physics {

	struct World_position;

	class Physics_comp : public ecs::Component<Physics_comp> {
		public:
			static constexpr const char* name() {return "Physics";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Physics_comp(ecs::Entity& owner,
						 Distance body_radius=Distance(1),
						 Mass mass=Mass(1), float restitution=1,
						 float friction=1, uint8_t group=1) noexcept;

			void accelerate_active(glm::vec2 dir)noexcept;
			void accelerate(Acceleration acc)noexcept;
			void impulse(Dir_force force)noexcept;
			void apply_force(Dir_force force)noexcept;
			void velocity(Velocity velocity)noexcept;

			void mass(Mass m) {
				_inv_mass = 1.f/m;
			}
			auto mass()const -> Mass {
				return 1.f/_inv_mass;
			}

			auto radius()const noexcept {return _body_radius;}
			void radius(Distance r)noexcept {_body_radius=r;}
			auto velocity()const noexcept {return _velocity;}
			auto acceleration()const noexcept {return _acceleration;}
			auto active()const noexcept {return _active;}


			struct Persisted_state;
		private:
			friend struct Persisted_state;
			friend class Physics_system;

			Distance _body_radius;
			Inv_mass _inv_mass;
			float _restitution;
			float _friction;
			Speed _max_active_velocity2 = Speed(0);
			Speed_per_time _active_acceleration = Speed_per_time(0);

			uint8_t _group = 1;
			uint8_t _group_exclude = 0;
			bool _active;

			Velocity _velocity;
			Acceleration _acceleration;
	};

}
}
}
