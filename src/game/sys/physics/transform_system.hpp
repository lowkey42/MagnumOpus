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
#include "../../level/level.hpp"


namespace mo {
namespace sys {
namespace physics {

	class Physics_comp;

	class Transform_system : private util::slot<ecs::Component_event> {
		public:
			Transform_system(
					ecs::Entity_manager& entity_manager, Distance max_entity_size,
					int world_width, int world_height,
			        const level::Level& world);

			void update(Time dt);

			template<typename F>
			void foreach_in_rect(Position top_left, Position bottom_right, F func);

			template<typename F>
			void foreach_in_range(Position pos, Angle dir, Distance near,
								  Distance max, Angle max_angle, Angle near_angle, F func);

			template<typename F>
			void foreach_in_cell(Position pos, F func);

			template<typename Pred>
			auto raycast_nearest_entity(Position pos, Angle dir,
										Distance max_distance,
										Pred pred)
					-> std::tuple<util::maybe<ecs::Entity&>, Distance>;

			template<typename FE, typename FW>
			void raycast(Position pos, Angle dir, Distance max_distance, FE on_entity, FW on_wall);

			/**
			 * Calls func exactly once for each unique pair of close entities (same or adjacent cell)
			 */
			template<typename F>
			void foreach_pair(F func);

			void pre_reload();
			void post_reload();


		private: // structures
			struct Cell_data {
				std::vector<ecs::Entity*> entities;

				void add(ecs::Entity& c);
				void remove(ecs::Entity& c);
			};

			void _on_comp_event(ecs::Component_event e);
			void _clamp_position(Position& p) {
				using namespace unit_literals;
				p = clamp(p, {0_m, 0_m}, {(_cells_x*_cell_size-1)*1_m, (_cells_y*_cell_size-1)*1_m});
			}
			inline uint16_t _get_cell_idx_for(Position pos) {
				const auto x = static_cast<uint16_t>(pos.x.value() / _cell_size);
				const auto y = static_cast<uint16_t>(pos.y.value() / _cell_size);

				return y*_cells_x + x;
			}
			inline Cell_data& _get_cell_for(Position pos) {
				return _cells.at(_get_cell_idx_for(pos));
			}

			const Distance _max_entity_size;
			const int _cell_size;
			const int _cells_x;
			const int _cells_y;
			ecs::Entity_manager& _em;
			Transform_comp::Pool& _pool;

			std::vector<Cell_data> _cells;
			const level::Level& _world;
	};

	template<typename F>
	void Transform_system::foreach_in_cell(Position pos, F func) {
		for( auto ep : _get_cell_for(pos).entities)
			func(*ep);
	}

	template<typename F>
	void Transform_system::foreach_in_rect(Position top_left, Position bottom_right, F func) {
		using util::range;

		top_left-=_max_entity_size;      //< take overlap into account
		bottom_right+=_max_entity_size;

		auto xb = static_cast<int32_t>(clamp(top_left.x.value()/_cell_size, 0, _cells_x));
		auto yb = static_cast<int32_t>(clamp(top_left.y.value()/_cell_size, 0, _cells_y));
		auto xe = static_cast<int32_t>(clamp(bottom_right.x.value()/_cell_size+1, 0, _cells_x));
		auto ye = static_cast<int32_t>(clamp(bottom_right.y.value()/_cell_size+1, 0, _cells_y));

		for(auto y : range(yb,ye-1)) {
			for(auto x : range(xb,xe-1)) {
				for(auto ep :  _cells.at(y*_cells_x +x).entities) {
					func(*ep);
				}
			}
		}
	}

	template<typename FE, typename FW>
	void Transform_system::raycast(Position start, Angle dir,
								   Distance max_distance, FE on_entity, FW on_wall) {
		using namespace unit_literals;
		using namespace util;
		using namespace glm;

		auto step = rotate(Position{0.1_m, 0_m}, dir);

		auto p=start;
		auto dist=0_m;
		for(; dist<=max_distance; dist+=0.1_m, p+=step) {
			auto x = static_cast<int32_t>(p.x.value()+0.5f);
			auto y = static_cast<int32_t>(p.y.value()+0.5f);
			if(_world.solid_real(p.x.value(), p.y.value())) {
				on_wall(x,y,dist.value());
				break;
			}
		}

		auto dist_2 = dist.value() * dist.value();
		auto end = step * dist.value();

		auto b = Position{min(start.x, end.x), min(start.y, end.y)};
		auto e = Position{max(start.x, end.x), max(start.y, end.y)};

		foreach_in_rect(b, e, [&](ecs::Entity& e){
			process(e.get<Transform_comp>(), e.get<Physics_comp>())
				>> [&](Transform_comp& trans, Physics_comp& phy) {

				auto s = remove_units(start);
				auto d = rotate(vec2{1,0}, dir);
				auto c = remove_units(trans.position());
				auto r = phy.radius().value();

				if(glm::length2(s-c)-r*r<=dist_2) {
					auto p = (dot(d, s-c) + dot(s-c, d))
							/ dot(d,d);
					auto q = (dot(s-c, s-c) - r)
							/ dot(d,d);

					auto in_root = (p*p)/4 - q;

					if(in_root>=0) {
						auto rv = glm::sqrt(in_root);
						auto dist1 = -p/2 + rv;
						auto dist2 = -p/2 - rv;

						if(dist2>0 && dist2<dist1)
							dist1 = dist2;

						if(dist1>0) {
							on_entity(e, dist1);
						}
					}
				}
			};
		});
	}

	template<typename Pred>
	auto Transform_system::raycast_nearest_entity(Position pos, Angle dir,
												  Distance max_distance,
												  Pred pred)
					-> std::tuple<util::maybe<ecs::Entity&>, Distance> {
		auto ret = util::maybe<ecs::Entity&>{};
		auto dist = max_distance.value();

		raycast(pos, dir, max_distance, [&](ecs::Entity& e, float d) {
			if(d<=dist && pred(e)) {
				dist = d;
				ret = e;
			}
		}, [&](int32_t x, int32_t y, float d) {
			if(d<=dist) {
				dist = d;
				ret = util::nothing();
			}
		});

		return std::make_pair(ret, Distance(dist));
	}


	template<typename F>
	void Transform_system::foreach_in_range(Position pos, Angle dir, Distance near,
											Distance max, Angle max_angle,
											Angle near_angle, F func) {
		using namespace unit_literals;

		const auto max_p = Position{max, max};
		const auto near_2 = near.value()*near.value();
		const auto max_2 = max.value()*max.value();

		foreach_in_rect(pos-max_p, pos+max_p, [&](ecs::Entity& e){
			e.get<Transform_comp>().process([&](const auto& trans){
				auto p = trans.position();
				auto diff = remove_units(p-pos);
				auto distance_2 = glm::length2(diff);

				auto target_dir = Angle(atan2(diff.y, diff.x));

				auto dir_diff = std::abs(normalize(dir-target_dir));
				if(dir_diff>180_deg)
					dir_diff = 360_deg - dir_diff;
				dir_diff = std::abs(dir_diff);

				if(distance_2<=max_2) {
					Angle a = distance_2<near_2 ? near_angle : max_angle;

					if(dir_diff<=a/2.f) {
						auto distance = glm::sqrt(distance_2);

						if(distance>0) {
							auto step = diff / distance;
							auto mp = pos;
							for(float d=0; d<=distance; d++) {
								mp+=step;
								if(this->_world.solid(mp.x.value()+0.5f, mp.y.value()+0.5f))
									return;
							}
						}

						func(e);
					}
				}
			});
		});
	}

	template<typename F>
	void Transform_system::foreach_pair(F func) {
		using util::range;

		for(auto y : range(_cells_y)) {
			for(auto x : range(_cells_x)) {
				auto& ce = _cells.at(y*_cells_x + x).entities;
				for( auto a=ce.begin(); a!=ce.end(); ++a) {
					// compare with current cell
					for( auto b=a+1; b!=ce.end(); ++b) {
						func(**a, **b);
					}

					// compare with surrounding cells
					for(auto ny : range(std::max(0,y-1), y)) {
						for(auto nx : range(std::max(0,x-1), x)) {
							if(ny==y && nx==x)
								continue;

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
