#include "camera_target_comp.hpp"
#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

namespace game {
namespace sys {
namespace cam {

	using namespace core;
	using namespace core::unit_literals;



	struct Camera_target_comp::Persisted_state {
		float mass, damping, stiffness, lazyness;

		Persisted_state(const Camera_target_comp& c)
				: mass(c._mass.value()), damping(c._damping), stiffness(c._stiffness), lazyness(c._lazyness) {}
	};

	sf2_structDef(Camera_target_comp::Persisted_state,
		sf2_member(mass),
		sf2_member(damping),
		sf2_member(stiffness),
		sf2_member(lazyness)
	)

	void Camera_target_comp::load(core::ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_mass = Mass(s.mass);
		_damping = s.damping;
		_stiffness = s.stiffness;
		_lazyness = s.lazyness;
	}
	void Camera_target_comp::store(core::ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

	void Camera_target_comp::chase(Position target, core::Time dt) {
		if(_unset) {
			_cam_pos = target;
			_unset = false;
		}

		auto diff = (target-_cam_pos);

		if(_sleeping && glm::length(remove_units(diff))<=_lazyness)
			return;

		_sleeping = false;

		core::Dir_force force = remove_units((diff * _stiffness)/second - (_velocity*_damping)) * 1_N;

		core::Acceleration accel = force / _mass;

		_velocity += accel*dt;

		if(glm::length(remove_units(_velocity))<0.1f)
			_sleeping = true;

		_cam_pos+= _velocity*dt;
	}
}
}
}

