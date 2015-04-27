/**************************************************************************\
 * information about a collision                                          *
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

#include "../../../core/utils/log.hpp"
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

}
}
}
