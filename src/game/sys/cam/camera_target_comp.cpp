#include "camera_target_comp.hpp"
#include <sf2/sf2.hpp>
#include "../../../core/utils/math.hpp"

namespace mo {
namespace sys {
namespace cam {

	using namespace unit_literals;


	void Camera_target_comp::load(sf2::JsonDeserializer& state,
	                              asset::Asset_manager&) {
		float mass_f = _mass / 1_kg;

		state.read_virtual(
			sf2::vmember("mass", mass_f),
			sf2::vmember("damping", _damping),
			sf2::vmember("freq", _freq),
			sf2::vmember("lazyness", _lazyness)
		);

		_mass = mass_f * 1_kg;
	}
	void Camera_target_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("mass", _mass / 1_kg),
			sf2::vmember("damping", _damping),
			sf2::vmember("freq", _freq),
			sf2::vmember("lazyness", _lazyness)
		);
	}

	void Camera_target_comp::chase(Position target, Time dt) {
		if(_unset) {
			_cam_pos = target;
			_unset = false;
		}

		auto diff = (target-_cam_pos);

		if(_sleeping<=0_s && glm::length(remove_units(diff))<=_lazyness)
			return;

		std::tie(_cam_pos, _velocity) = util::spring(_cam_pos,
													 _velocity,
													 target,
													 _damping,
													 _freq,
													 dt);

		if(glm::length(remove_units(_velocity))<0.1f)
			_sleeping = _sleeping - dt;
		else
			_sleeping = 500_ms;
	}
	void Camera_target_comp::reset() {
		_unset = true;
	}
}
}
}

