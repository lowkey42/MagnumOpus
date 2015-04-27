#include "transform_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

namespace mo {
namespace sys {
namespace physics {

	struct Transform_comp::Persisted_state {
		float x, y, rot;

		Persisted_state(const Transform_comp& c)
				: x(c._position.x.value()), y(c._position.y.value()), rot(c._rotation.value()) {}
	};

	sf2_structDef(Transform_comp::Persisted_state,
		sf2_member(x),
		sf2_member(y),
		sf2_member(rot)
	)

	void Transform_comp::load(ecs::Entity_state& state){
		auto s = state.read_to(Persisted_state{*this});
		_position.x = Distance(s.x);
		_position.y = Distance(s.y);
		_rotation = Angle(s.rot);
		_dirty = true;
	}
	void Transform_comp::store(ecs::Entity_state& state){
		state.write_from(Persisted_state{*this});
	}

}
}
}
