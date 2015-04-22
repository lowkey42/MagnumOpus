/*********************************************************************************\
 * Provides fast access to entities base on their position                       *
 *                                               ___                             *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___            *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|           *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \           *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/           *
 *                |___/                              |_|                         *
 *                                                                               *
 * Copyright (c) 2014 Florian Oetke                                              *
 *                                                                               *
 *  This file is part of MagnumOpus and distributed under the MIT License        *
 *  See LICENSE file for details.                                                *
\*********************************************************************************/

#pragma once

#include <functional>
#include <vector>
#include <array>
#include <glm/gtx/norm.hpp>

#include "../../../core/utils/template_utils.hpp"
#include "transform_comp.hpp"
#include "physics_comp.hpp"


namespace game {
namespace sys {
namespace physics {

	class Physics_comp;

	class Transform_system : private core::util::slot<core::ecs::Component_event> {
		public:
			Transform_system(
					core::ecs::Entity_manager& entity_manager, core::Distance max_entity_size,
					int world_height, int world_width);

			void update(core::Time dt);

			template<typename F>
			void foreach_in_rect(core::Position top_left, core::Position bottom_right, F func);

			template<typename F>
			void foreach_in_range(core::Position pos, core::Angle dir, core::Distance near,
			                      core::Distance max, core::Angle max_angle, core::Angle near_angle, F func);

			template<typename F>
			void foreach_in_cell(core::Position pos, F func);

			auto raycast_nearest_entity(core::Position pos, core::Angle dir,
			                            core::Distance max_distance) -> core::util::maybe<core::ecs::Entity&>;

			template<typename FE, typename FW>
			void raycast(core::Position pos, core::Angle dir, core::Distance max_distance, FE on_entity, FW on_wall);

			/**
			 * Calls func exactly once for each unique pair of close entities (same or adjacent cell)
			 */
			template<typename F>
			void foreach_pair(F func);

			void pre_reload();
			void post_reload();


		private: // structures
			struct Cell_data {
				std::vector<core::ecs::Entity*> entities;

				void add(core::ecs::Entity& c);
				void remove(core::ecs::Entity& c);
			};

			void _on_comp_event(core::ecs::Component_event e);
			void _clamp_position(core::Position& p) {
				using namespace core::unit_literals;
				p = clamp(p, {0_m, 0_m}, {(_cells_x*_cell_size-1)*1_m, (_cells_y*_cell_size-1)*1_m});
			}
			inline uint16_t _get_cell_idx_for(core::Position pos) {
				const auto x = static_cast<uint16_t>(pos.x.value() / _cell_size);
				const auto y = static_cast<uint16_t>(pos.y.value() / _cell_size);

				return y*_cells_x + x;
			}
			inline Cell_data& _get_cell_for(core::Position pos) {
				return _cells.at(_get_cell_idx_for(pos));
			}

			const core::Distance _max_entity_size;
			const int _cell_size;
			const int _cells_x;
			const int _cells_y;
			core::ecs::Entity_manager& _em;
			Transform_comp::Pool& _pool;

			std::vector<Cell_data> _cells;

	};

	template<typename F>
	void Transform_system::foreach_in_cell(core::Position pos, F func) {
		for( auto ep : _get_cell_for(pos).entities)
			func(*ep);
	}

	template<typename F>
	void Transform_system::foreach_in_rect(core::Position top_left, core::Position bottom_right, F func) {
		using core::util::range;

		top_left-=_max_entity_size;      //< take overlap into account
		bottom_right+=_max_entity_size;

		auto xb = static_cast<int32_t>(core::clamp(top_left.x.value()/_cell_size, 0, _cells_x));
		auto yb = static_cast<int32_t>(core::clamp(top_left.y.value()/_cell_size, 0, _cells_y));
		auto xe = static_cast<int32_t>(core::clamp(bottom_right.x.value()/_cell_size+1, 0, _cells_x));
		auto ye = static_cast<int32_t>(core::clamp(bottom_right.y.value()/_cell_size+1, 0, _cells_y));

		for(auto y : range(yb,ye-1)) {
			for(auto x : range(xb,xe-1)) {
				for(auto ep :  _cells.at(y*_cells_x +x).entities) {
					func(*ep);
				}
			}
		}
	}

	template<typename FE, typename FW>
	void Transform_system::raycast(core::Position start, core::Angle dir,
	                               core::Distance max_distance, FE on_entity, FW on_wall) {
		using namespace core::unit_literals;

		core::Position end = core::rotate(core::Position{0_m, max_distance}, dir);

		auto b = core::Position{min(start.x, end.x), min(start.y, end.y)};
		auto e = core::Position{max(start.x, end.x), max(start.y, end.y)};

		foreach_in_rect(b, e, [&](core::ecs::Entity& e){
		//	doIf(e.get<TransformComp>(), e.get<PhysicsComp>(),
		//	     [&](TransformComp& t, PhysicsComp& p){

				// TODO
		//	});
		//	e.get<TransformComp>().doIf([&](const TransformComp& trans){

		});
	}


	template<typename F>
	void Transform_system::foreach_in_range(core::Position pos, core::Angle dir, core::Distance near,
	                                        core::Distance max, core::Angle max_angle,
											core::Angle near_angle, F func) {
		auto in_cone = [](core::Angle dir, core::Position diff, float dist2, core::Angle a) {
			auto da = core::Angle(glm::acos(diff.y.value()/glm::sqrt(dist2)));
			return between(da, dir-a/2.f, dir+a/2.f);
		};

		const auto max_p = core::Position{max, max};
		const auto near_2 = near.value()*near.value();
		const auto max_2 = max.value()*max.value();

		foreach_in_rect(pos-max_p, pos+max_p, [&](core::ecs::Entity& e){
			e.get<Transform_comp>().process([&](const auto& trans){
				auto p = trans.position();
				auto diff = p-pos;
				auto dist_2 = glm::length2(remove_units(diff));

				if(dist_2<=max_2) {
					core::Angle a = dist_2<near_2 ? near_angle : max_angle;

					// FIXME: check for occlusion
					if(in_cone(dir, diff, dist_2, a))
						func(e);
				}
			});
		});
	}

	template<typename F>
	void Transform_system::foreach_pair(F func) {
		using core::util::range;

		for(auto y : range(_cells_y)) {
			for(auto x : range(_cells_x)) {
				auto& ce = _cells.at(y*_cells_x + x).entities;
				for( auto a=ce.begin(); a!=ce.end(); ++a) {
					// compare with current cell
					for( auto b=a+1; b!=ce.end(); ++b) {
						func(**a, **b);
					}

					// compare with surrounding cells
					for(auto ny : range(std::max(0,y-1), std::min(_cells_y,y-1)-1)) {
						for(auto nx : range(std::max(0,x-1), std::min(_cells_x,x-1)-1)) {
							auto& nce = _cells.at(ny*_cells_x + nx).entities;
							for( auto& b : nce) {
								func(**a, *b);
							}
						}
					}
				}
			}
		}
	}

}
}
}
