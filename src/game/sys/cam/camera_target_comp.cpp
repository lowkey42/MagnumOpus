#include "camera_target_comp.hpp"
#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"
#include "../../../core/utils/math.hpp"

namespace game {
namespace sys {
namespace cam {

	using namespace core;
	using namespace core::unit_literals;



	struct Camera_target_comp::Persisted_state {
		float mass, damping, freq, lazyness;

		Persisted_state(const Camera_target_comp& c)
				: mass(c._mass.value()), damping(c._damping),
		          freq(c._freq), lazyness(c._lazyness) {}
	};

	sf2_structDef(Camera_target_comp::Persisted_state,
		sf2_member(mass),
		sf2_member(damping),
		sf2_member(freq),
		sf2_member(lazyness)
	)

	void Camera_target_comp::load(core::ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_mass = Mass(s.mass);
		_damping = s.damping;
		_freq = s.freq;
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

		if(_sleeping<=0_s && glm::length(remove_units(diff))<=_lazyness)
			return;

		std::tie(_cam_pos, _velocity) = core::util::spring(_cam_pos,
		                                                   _velocity,
		                                                   target,
		                                                   _damping,
		                                                   _freq,
		                                                   dt);

		if(glm::length(remove_units(_velocity))<0.1f)
			_sleeping = _sleeping - dt;
		else
			_sleeping = dt*4;
	}
}
}
}

