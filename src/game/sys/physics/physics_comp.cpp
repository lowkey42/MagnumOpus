#include "physics_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

namespace mo {
namespace sys {
namespace physics{

	struct DFloatS {
		float x, y;
	};
	sf2_structDef(DFloatS,
		sf2_member(x),
		sf2_member(y)
	)

	struct Physics_comp::Persisted_state {
		float radius;
		float mass;
		float restitution;
		float friction;

		bool solid;

		DFloatS velocity;
		DFloatS acceleration;

		Persisted_state(const Physics_comp& c) :
			radius(c._body_radius.value()),
			mass(c.mass().value()),
			restitution(c._restitution),
			friction(c._friction),
			solid(c._solid),
			velocity(DFloatS{c._velocity.x.value(), c._velocity.y.value()}),
			acceleration(DFloatS{c._acceleration.x.value(), c._acceleration.y.value()}) {
		}
	};

	sf2_structDef(Physics_comp::Persisted_state,
		sf2_member(radius),
		sf2_member(mass),
		sf2_member(restitution),
		sf2_member(friction),
		sf2_member(solid),
		sf2_member(velocity),
		sf2_member(acceleration)
	)

	void Physics_comp::load(ecs::Entity_state& state){
		auto s = state.read_to(Persisted_state{*this});

		_body_radius = Distance(s.radius);
		mass(Mass(s.mass));
		_restitution = s.restitution;
		_friction = s.friction;
		_solid = s.solid;
		_velocity = {Speed(s.velocity.x), Speed(s.velocity.y)};
		_acceleration = {Speed_per_time(s.acceleration.x), Speed_per_time(s.acceleration.y)};
		_active = true;
	}
	void Physics_comp::store(ecs::Entity_state& state){
		state.write_from(Persisted_state{*this});
	}

}
}
}
