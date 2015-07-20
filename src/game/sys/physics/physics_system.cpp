#define GLM_SWIZZLE

#include "physics_system.hpp"

#include<glm/gtc/constants.hpp>

namespace mo {
namespace sys {
namespace physics {
	using namespace std::placeholders;
	using namespace unit_literals;


	constexpr float G = 10;

	Physics_system::Physics_system(
			ecs::Entity_manager& entity_manager, Transform_system& ts,
			Distance min_body_size, Speed max_body_velocity,
			const level::Level& world)
		: _world(world), _min_body_size(min_body_size),
		  _max_body_velocity(max_body_velocity),
		  _sub_step_time(std::min(min_body_size.value()/(2*max_body_velocity.value()), 1.f/60)),
		  _max_steps_per_frame(static_cast<int>((7.f/_sub_step_time.value())/60 *4)),
		  _physics_pool(entity_manager.list<Physics_comp>()),
		  _transform_sys(ts), _dt_acc(0) {
		entity_manager.register_component_type<physics::Physics_comp>();
	}

	void Physics_system::_on_collision(Manifold& m) {
		collisions.inform(m);

		if(m.is_with_object()) {
			auto m_inv = m.inverse();
			collisions.inform(m_inv);
		}
	}

	void Physics_system::update(Time dt) {
		_dt_acc+=dt;

		int steps = std::min(static_cast<int>(_dt_acc/_sub_step_time), _max_steps_per_frame);

		for(int i=0; i<steps; i++)
			_step(i==steps-1);

		_dt_acc = std::max(_dt_acc- steps*_sub_step_time, Time(0));
	}

	void Physics_system::_step(bool last_step) {
		_manifold_buffer.clear();

		// collect all manifolds
		_transform_sys.foreach_pair([&](ecs::Entity& a, ecs::Entity& b) {
			auto apm = a.get<Physics_comp>();
			auto bpm = b.get<Physics_comp>();

			if(apm.is_some() && bpm.is_some()) {
				auto& ap = apm.get_or_throw();
				auto& bp = bpm.get_or_throw();

				if(ap.active() || bp.active()) {
					auto manifold = _check_collision(ap, bp);
					manifold.process([&](Manifold& m) {
						_manifold_buffer.push_back(m);
					});
				}
			}
		});

		for(auto& pc : _physics_pool) {
			if(pc.active()) {
				_step_entity(pc, last_step);
				_check_env_collisions(pc, _manifold_buffer);
			}
		}

		for(auto& m : _manifold_buffer) {
			_solve_collision(m);
			_on_collision(m);
		}
	}

	void Physics_system::_step_entity(Physics_comp& self, bool last_step) {
		Time dt = _sub_step_time;

		if(!self.active())
			return;

		auto oldVelocity = self._velocity;

		if(!is_zero(self._acceleration)) {
			self._velocity+=self._acceleration*dt;
			auto vx = self._velocity.x.value();
			auto vy = self._velocity.y.value();
			auto speed = Speed(vx*vx + vy*vy);
			if(speed>_max_body_velocity) {
				self._velocity=self._velocity * (_max_body_velocity/speed);
			}

		} else if(is_zero(self._velocity)) {
			self._active = false;
			return;
		}

		auto transform = self.owner().get<Transform_comp>();

		transform.process([&](auto& tc){
			auto pos = tc.position();
			pos+=oldVelocity*dt;

			tc.position(pos);

			if(last_step) {
				int world_x = static_cast<int>(pos.x.value());
				int world_y = static_cast<int>(pos.y.value());

				// apply friction
				auto fric_speed = self._friction*_world.friction(world_x, world_y) *G;
				self._velocity+=Velocity{
						-sign(self._velocity.x) * std::min(fric_speed, std::abs(self._velocity.x.value())),
						-sign(self._velocity.y) * std::min(fric_speed, std::abs(self._velocity.y.value()))
				};

				// reset acceleration
			   self._acceleration=self._acceleration*0.0f;
			}
		});
	}

	void Physics_system::_check_env_collisions(Physics_comp& a, std::vector<Manifold>& buffer) {
		auto pos = a.owner().get<Transform_comp>().get_or_throw().position();
		auto radius = a.radius().value();

		auto min_world_x = std::max(static_cast<int>(std::floor(pos.x.value()- radius)), 0);
		auto min_world_y = std::max(static_cast<int>(std::floor(pos.y.value()- radius)), 0);
		auto max_world_x = std::min(static_cast<int>(std::ceil(pos.x.value()+ radius)), _world.width()-1);
		auto max_world_y = std::min(static_cast<int>(std::ceil(pos.y.value()+ radius)), _world.height()-1);


		for(auto x : util::range(min_world_x, max_world_x)) {
			for(auto y : util::range(min_world_y, max_world_y)) {
				if(_world.solid(x,y)) {
					auto tile_dim = _world.get(x,y).dimensions();

					auto n = Position(x,y) - pos;

					auto closest = n;

					closest = glm::clamp(remove_units(closest), -tile_dim.zw(), -tile_dim.xy()) * 1_m;
					bool inside = false;

					// Circle is inside the AABB, so we need to clamp the circle's center
					// to the closest edge
					if(n==closest) {
						inside = true;

						// Find closest axis
						if(abs(n.x) > abs(n.y)) {
							closest.x = closest.x > 0_m ? tile_dim.z*1_m : tile_dim.x*1_m;

						// y axis is shorter
						} else {
							closest.y = closest.y > 0_m ? tile_dim.w*1_m : tile_dim.y*1_m;
						}
					}

					auto diff = remove_units(n-closest);


					auto dist_sqr = (diff.x*diff.x + diff.y*diff.y);

					if(dist_sqr > (radius*radius) && !inside )
						continue;

					float dist = glm::sqrt(dist_sqr);

					auto normal = dist>0 ? Position(diff / dist) : Position(1_m, 0_m);

					if(inside)
						normal=-normal;

					Distance penetration = abs((dist-radius) * 1_m);

					buffer.emplace_back(a, x,y, penetration, normal);
				}
			}
		}

	}

	auto Physics_system::_check_collision(Physics_comp& a, Physics_comp& b) -> util::maybe<Manifold> {
		if(a._group_exclude & b._group_exclude)
			return util::nothing();

		auto diff = remove_units(a.owner().get<Transform_comp>().get_or_throw().position() - b.owner().get<Transform_comp>().get_or_throw().position());
		auto rs = (a.radius()+b.radius()).value();
		auto dist_sqr = (diff.x*diff.x + diff.y*diff.y);

		if( dist_sqr <= (rs*rs)) {
			float dist = glm::sqrt(dist_sqr);
			return dist>0 ?
						Manifold(a, b, abs(Distance(rs-dist)), -diff/dist) :
						Manifold(a, b, a.radius(), Position(1_m, 0_m));
		} else
			return util::nothing();
	}

	void Physics_system::_solve_collision(Manifold& m) {
		if(!m) return;

		// Calculate relative velocity
		auto relVel = -m.a->_velocity;
		float e = m.a->_restitution;
		auto inv_mass_sum = m.a->_inv_mass;

		if(m.is_with_object()) {
			if((m.a->_group & m.b.comp->_group) == 0)
				return;

			relVel+=m.b.comp->_velocity;
			e = std::max(e, m.b.comp->_restitution);
			inv_mass_sum+=m.b.comp->_inv_mass;
		}

		// Calculate relative velocity in terms of the normal direction
		float vel_along_normal = glm::dot(remove_units(relVel), remove_units(m.normal));

		// Do not resolve if velocities are separating
		if(vel_along_normal > 0)
		  return;


		// Calculate impulse scalar
		Force imp_force = Speed_per_time(-(1 + e) * vel_along_normal) / inv_mass_sum;

		const auto resp_force = remove_units(m.normal) * imp_force;

		// apply solution
		m.a->impulse(-resp_force);

		if(m.is_with_object()) {
			m.b.comp->impulse(resp_force);
		}

		// positional correction
		constexpr float percent = 0.2;
		constexpr float slop = 0.1;
		if(m.penetration.value() - slop > 0) {
			auto correction = m.normal * ((m.penetration.value() - slop) / inv_mass_sum.value() * percent);

			auto aPos = m.a->owner().get<Transform_comp>().get_or_throw().position();
			aPos-= correction*m.a->_inv_mass.value();
			m.a->owner().get<Transform_comp>().get_or_throw().position(aPos);

			if(m.is_with_object()) {
				auto bPos = m.b.comp->owner().get<Transform_comp>().get_or_throw().position();
				bPos+= correction*m.b.comp->_inv_mass.value();
				m.b.comp->owner().get<Transform_comp>().get_or_throw().position(bPos);
			}
		}
	}

}
}
}
