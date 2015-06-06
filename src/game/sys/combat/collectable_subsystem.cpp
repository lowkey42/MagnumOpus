#include "collectable_subsystem.hpp"

#include <core/asset/asset_manager.hpp>

#include "score_comp.hpp"
#include "collector_comp.hpp"

#include <core/renderer/particles.hpp>

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	Collectable_subsystem::Collectable_subsystem(
	        asset::Asset_manager& assets,
	        ecs::Entity_manager& entity_manager,
	        physics::Transform_system& transform,
	        renderer::Particle_renderer& particles)
	    : _assets(assets),
	      _em(entity_manager),
	      _ts(transform),
	      _particles(particles),
	      _collectors(entity_manager.list<Collector_comp>()) {

	}

	void Collectable_subsystem::update(Time dt) {
		for(auto& c : _collectors) {
			if(c._active) {
				c._active = false;

				c.owner().get<physics::Transform_comp>().process([&](auto& t){
					if(!c._particles) {
						c._particles = _particles.create_emiter(
								t.position(),
								0_deg,
								0.1_m,
								renderer::Collision_handler::kill,
								200,
								100,
								0.25_s, 0.75_s,
								util::cerp<Angle>({0_deg}, c._far_angle/2.f),
								util::scerp<Angle>(0_deg, 0_deg),
								util::lerp<Speed_per_time>(30_m/second_2, 10_m/second_2),
								util::lerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
								util::lerp<glm::vec4>({0.25,0.25,0.25,0}, {0,0,0,0}, {0,0,0,0}),
								util::lerp<Position>({50_cm, 50_cm}, {5_cm, 5_cm}, {2_cm, 2_cm}),
								util::lerp<int8_t>(0, 0),
								_assets.load<renderer::Texture>("tex:particle_line.png"_aid),
								true
						);
					}

					if(c._particles) {
						c._particles->update_center(t.position(), t.rotation());
						c._particles->active(true);
					}

					_ts.foreach_in_range(t.position(), t.rotation(),
					                     c._near, c._far,
					                     c._far_angle, c._near_angle,
					                    [&](ecs::Entity& e){
						util::process(e.get<physics::Transform_comp>(),
						              e.get<physics::Physics_comp>())
						>> [&](auto& tt, auto& tp){
							tp.apply_force(c._force * remove_units(t.position()-tt.position()) );
						};
					});

				});
			} else {
				if(c._particles) {
					c._particles->active(false);
				}
			}
		}
	}

	void Collectable_subsystem::_on_collision(physics::Manifold& m) {
		if(m.is_with_object()) {
			m.a->owner().get<Score_comp>().process([&](auto& s){
				if(s._collectable && !s._collected) {
					m.b.comp->owner().get<Score_comp>().process([&](auto& os) {
						if(os._collector) {
							os._value+=s._value;
							s._collected = true;
							_em.erase(s.owner_ptr());
						}
					});
				}
			});
		}

	}

}
}
}
