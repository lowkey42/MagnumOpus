#include "collector_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

#include "../../physics/physics_comp.hpp"
#include "../../physics/transform_system.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	struct Collector_comp::Persisted_state {
		float force;
		float near, far;
		float near_angle, far_angle;

		Persisted_state(const Collector_comp& c)
		    : force(c._force.value()),
		      near(c._near.value()),
		      far(c._far.value()),
		      near_angle(c._near_angle / 1_deg),
		      far_angle(c._far_angle / 1_deg) {}
	};

	sf2_structDef(Collector_comp::Persisted_state,
		sf2_member(force),
		sf2_member(near),
		sf2_member(far),
		sf2_member(near_angle),
		sf2_member(far_angle)
	)

	void Collector_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_force      = s.force * 1_N;
		_near       = s.near * 1_m;
		_far        = s.far * 1_m;
		_near_angle = s.near_angle * 1_deg;
		_far_angle  = s.far_angle * 1_deg;
	}
	void Collector_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
