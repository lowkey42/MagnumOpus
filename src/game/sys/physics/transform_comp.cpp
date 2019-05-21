#include "transform_comp.hpp"

#include <sf2/sf2.hpp>

namespace mo {
namespace sys {
namespace physics {

	using namespace unit_literals;

	struct Transform_comp::Persisted_state {
		float x, y, rot, rot_speed, layer;
		bool rotation_fixed;

		Persisted_state(const Transform_comp& c)
				: x(c._position.x.value()), y(c._position.y.value()),
		          rot(c._rotation.value()), rot_speed(c._max_rotation_speed/ (1_deg/second)),
		          layer(c._layer), rotation_fixed(c._rotation_fixed) {}
	};

	sf2_structDef(Transform_comp::Persisted_state,
		x,
		y,
		rot,
		rot_speed,
		layer,
		rotation_fixed
	)

	void Transform_comp::load(sf2::JsonDeserializer& state,
	                          asset::Asset_manager&){
		auto s = Persisted_state{*this};
		state.read(s);

		_position.x = Distance(s.x);
		_position.y = Distance(s.y);
		_rotation = Angle(s.rot);
		_max_rotation_speed = s.rot_speed * (1_deg/second);
		_rotation_fixed = s.rotation_fixed;
		layer(s.layer);
		_dirty = true;
	}
	void Transform_comp::save(sf2::JsonSerializer& state)const {
		auto s = Persisted_state{*this};
		state.write(s);
	}

}
}
}
