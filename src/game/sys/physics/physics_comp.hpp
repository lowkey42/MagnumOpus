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
#include "transform_comp.hpp"

#include "../../../core/ecs/ecs.hpp"
#include "../../../core/units.hpp"

namespace mo {
namespace sys {
namespace physics {

	class Physics_comp;
	struct World_position;

	struct Manifold {
		enum class Type {
			none,
			object,
			environment
		} type;

		Physics_comp* a;

		union B_obj_union {
			Physics_comp* comp;
			struct{int x,y;} pos;

			B_obj_union(Physics_comp* b) : comp(b){}
			B_obj_union(int x, int y) : pos{x,y}{}
		} b;

		Distance penetration;
		Position normal;


		Manifold()
			: type(Type::none), a(nullptr), b(nullptr), penetration(0) {}

		Manifold(Physics_comp& a, Physics_comp& b, Distance penetration, Position normal)
			: type(Type::object), a(&a), b(&b), penetration(penetration), normal(normal) {}

		Manifold(Physics_comp& a, int bx, int by, Distance penetration, Position normal)
			: type(Type::environment), a(&a), b(bx,by), penetration(penetration), normal(normal) {}

		Manifold inverse()const noexcept {
			INVARIANT(type==Type::object, "inverse() makes no sense for environment collisions!");
			return Manifold(*b.comp, *a, penetration, normal);
		}
		bool is_with_environment()const noexcept {
			return type==Type::environment;
		}
		bool is_with_object()const noexcept {
			return type==Type::object;
		}
		bool is_valid()const noexcept {
			return type!=Type::none;
		}
		operator bool()const noexcept {
			return is_valid();
		}
	};

	class Physics_comp : public ecs::Component<Physics_comp> {
		public:
			static constexpr const char* name() {return "Physics";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Physics_comp(ecs::Entity& owner, Distance body_radius=Distance(1),
						Mass mass=Mass(1), float restitution=1, float friction=1, bool solid=true) noexcept
				: Component(owner),
				  _body_radius(body_radius),
				  _inv_mass(1.f/mass),
				  _restitution(restitution),
				  _friction(friction),
				  _solid(solid),
				  _active(true) {}

			void mass(Mass m) {
				_inv_mass = 1.f/m;
			}
			auto mass()const -> Mass {
				return 1.f/_inv_mass;
			}

			void accelerate(Acceleration acc)noexcept {
				_acceleration+=acc;
				if(!is_zero(acc))
					_active = true;
			}
			void impulse(Dir_force force)noexcept {
				_velocity+=_inv_mass*force*unit_literals::second;
				_active = true;
			}
			void apply_force(Dir_force force)noexcept {
				accelerate(_inv_mass*force);
			}
			void velocity(Velocity velocity)noexcept {
				_velocity = velocity;
				if(!is_zero(velocity))
					_active = true;
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

			bool _solid;
			bool _active;

			Velocity _velocity;
			Acceleration _acceleration;
	};

}
}
}
