#include "transform_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

namespace mo {
namespace sys {
namespace physics {

	using namespace unit_literals;

	struct Transform_comp::Persisted_state {
		float x, y, rot, rot_speed, layer;

		Persisted_state(const Transform_comp& c)
				: x(c._position.x.value()), y(c._position.y.value()),
		          rot(c._rotation.value()), rot_speed(c._max_rotation_speed/ (1_deg/second)),
		          layer(c._layer){}
	};

	sf2_structDef(Transform_comp::Persisted_state,
		sf2_member(x),
		sf2_member(y),
		sf2_member(rot),
		sf2_member(rot_speed),
		sf2_member(layer)
	)

	void Transform_comp::load(ecs::Entity_state& state){
		auto s = state.read_to(Persisted_state{*this});
		_position.x = Distance(s.x);
		_position.y = Distance(s.y);
		_rotation = Angle(s.rot);
		_max_rotation_speed = s.rot_speed * (1_deg/second);
		layer(s.layer);
		_dirty = true;
	}
	void Transform_comp::store(ecs::Entity_state& state){
		state.write_from(Persisted_state{*this});
	}

}
}
}
