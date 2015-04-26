#include "transform_system.hpp"
#include "physics_comp.hpp"

#include<glm/gtc/constants.hpp>

namespace game {
namespace sys {
namespace physics {
	using namespace core;
	using namespace std::placeholders;
	using namespace unit_literals;

	namespace {
		/// source: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
		uint32_t next_pot(uint32_t n) {
			n--;
			n |= n >> 1;
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			n++;

			return n;
		}
		int divide_ceil(int a, int b) {
			return static_cast<int>(std::ceil(
				  static_cast<float>(a)/b
			));
		}
		int calc_cell_size(Distance max_entity_size) {
			return next_pot(static_cast<uint32_t>(std::ceil(max_entity_size.value()*2)));
		}
	}

	Transform_system::Transform_system(
			ecs::Entity_manager& entity_manager, Distance max_entity_size,
			int world_width, int world_height)
		: util::slot<ecs::Component_event>(&Transform_system::_on_comp_event, this),
	      _max_entity_size(max_entity_size),
		  _cell_size(calc_cell_size(max_entity_size)),
		  _cells_x(divide_ceil(world_width, _cell_size)), _cells_y(divide_ceil(world_height, _cell_size)),
		  _em(entity_manager), _pool(_em.list<Transform_comp>()),
		  _cells(_cells_y*_cells_x) {

		_em.register_component_type<physics::Transform_comp>();

		connect(entity_manager.list<Transform_comp>());
	}

	void Transform_system::update(Time) {
		for(auto& c : _pool) {
			if(c._dirty) {
				_clamp_position(c._position);
				c._dirty = false;

				auto old_cell_idx = c._cell_idx;
				auto new_cell_idx = _get_cell_idx_for(c._position);

				if(old_cell_idx!=new_cell_idx) {
					c._cell_idx = new_cell_idx;

					auto& new_cell = _cells.at(new_cell_idx);
					new_cell.add(c.owner());

					if(old_cell_idx>=0) {
						auto& old_cell = _cells.at(old_cell_idx);
						old_cell.remove(c.owner());
					}
				}
			}
		}
	}

	void Transform_system::_on_comp_event(ecs::Component_event e) {
		e.handle.get<Transform_comp>().process([&](Transform_comp& trans){
			if(e.type!=ecs::Component_event_type::created) {
				_get_cell_for(trans._position).remove(trans.owner());
			}
		});
	}
	void Transform_system::Cell_data::add(ecs::Entity& c) {
		entities.emplace_back(&c);
	}

	void Transform_system::Cell_data::remove(ecs::Entity& c) {
		using std::swap;

		auto found = std::find_if(entities.begin(), entities.end(),
								   [&](ecs::Entity* e){return e==&c;} );

		if(found!=entities.end()) {
			swap(*found, entities.back());
			entities.pop_back();
		}
	}

	void Transform_system::pre_reload() {
		for(auto& c : _cells)
			c.entities.clear();
	}

	void Transform_system::post_reload() {
	}

	util::maybe<ecs::Entity&> Transform_system::raycast_nearest_entity(Position pos, Angle dir, Distance max_distance) {
		auto ret = util::maybe<ecs::Entity&>{};
		auto dist = std::numeric_limits<float>::max();

		raycast(pos, dir, max_distance, [&](ecs::Entity& e, float d) {
			if(d<dist) {
				dist = d;
				ret = e;
			}
		}, [&](int32_t x, int32_t y, float d) {
			if(d<dist) {
				dist = d;
				ret = util::nothing();
			}
		});

		return ret;
	}

}
}
}
