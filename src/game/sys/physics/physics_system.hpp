/*******************************************************************************\
 * Handles physical interactions between selected entities                     *
 *                                               ___                           *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___          *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|         *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \         *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/         *
 *                |___/                              |_|                       *
 *                                                                             *
 * Copyright (c) 2014 Florian Oetke                                            *
 *                                                                             *
 *  This file is part of MagnumOpus and distributed under the MIT License      *
 *  See LICENSE file for details.                                              *
\*******************************************************************************/

#pragma once

#include <functional>
#include <vector>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include "physics_comp.hpp"
#include "transform_system.hpp"

namespace game {
namespace sys {
namespace physics {

	struct World_interface {
		virtual auto solid(int x, int y)const -> bool =0;
		virtual auto friction(int x, int y)const -> float =0;

		virtual auto width()const -> int =0;
		virtual auto height()const -> int =0;
	};

	class Physics_system {
		public:
			Physics_system(
					core::ecs::Entity_manager& entity_manager, Transform_system& ts,
					core::Distance min_body_size, core::Speed max_body_velocity, World_interface& world);

			void update(core::Time dt);

			core::util::signal_source<Manifold&> collisions;

		private:
			void _step(bool lastStep);
			void _on_collision(Manifold& m);

			void _step_entity(Physics_comp& physics, bool last_step=true);
			void _solve_collision(Manifold& m);
			auto _check_collision(Physics_comp& a, Physics_comp& b) -> core::util::maybe<Manifold>;
			void _check_env_collisions(Physics_comp& a, std::vector<Manifold>& buffer);

			const World_interface& _world;

			const core::Distance _min_body_size;
			const core::Speed _max_body_velocity;

			const core::Time _sub_step_time; //< derived from minBodySize and maxBodyVelocity

			const int _max_steps_per_frame;

			Physics_comp::Pool& _physics_pool;
			Transform_system& _transform_sys;

			core::Time _dt_acc;
			std::vector<Manifold> _manifold_buffer;
	};

}
}
}
