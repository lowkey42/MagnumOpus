#include "collector_comp.hpp"

#include "../physics/physics_comp.hpp"
#include "../physics/transform_system.hpp"

namespace mo {
namespace sys {
namespace item {

	using namespace unit_literals;


	void Collector_comp::load(sf2::JsonDeserializer& state,
	                          asset::Asset_manager&) {
		float force = _force / 1_n;
		float near = _near / 1_m;
		float far = _far / 1_m;
		float near_angle = _near_angle / 1_deg;
		float far_angle = _far_angle / 1_deg;

		state.read_virtual(
			sf2::vmember("force", force),
			sf2::vmember("near", near),
			sf2::vmember("far", far),
			sf2::vmember("near_angle", near_angle),
			sf2::vmember("far_angle", far_angle)
		);

		_force      = force * 1_n;
		_near       = near * 1_m;
		_far        = far * 1_m;
		_near_angle = near_angle * 1_deg;
		_far_angle  = far_angle * 1_deg;
	}
	void Collector_comp::save(sf2::JsonSerializer& state)const {
		float force = _force / 1_n;
		float near = _near / 1_m;
		float far = _far / 1_m;
		float near_angle = _near_angle / 1_deg;
		float far_angle = _far_angle / 1_deg;

		state.write_virtual(
			sf2::vmember("force", force),
			sf2::vmember("near", near),
			sf2::vmember("far", far),
			sf2::vmember("near_angle", near_angle),
			sf2::vmember("far_angle", far_angle)
		);
	}

}
}
}
