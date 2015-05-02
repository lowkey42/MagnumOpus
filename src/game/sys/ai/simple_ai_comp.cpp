#include "simple_ai_comp.hpp"

#include "../controller/controllable_comp.hpp"
#include "../physics/transform_comp.hpp"

#include "../../../core/units.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"


namespace mo {
namespace sys {
namespace ai {

	using namespace unit_literals;


	struct Simple_ai_comp::Persisted_state {
		float attack_distance, near, max, near_angle,
		      far_angle, follow_time;

		Persisted_state(const Simple_ai_comp& c)
				: attack_distance(c.attack_distance.value()),
		          near(c.near.value()), max(c.max.value()),
				  near_angle(c.near_angle.value() / (1_deg).value()),
				  far_angle(c.far_angle.value() / (1_deg).value()),
		          follow_time(c._follow_time.value()) {}
	};

	sf2_structDef(Simple_ai_comp::Persisted_state,
		sf2_member(attack_distance),
		sf2_member(near),
		sf2_member(max),
		sf2_member(near_angle),
		sf2_member(far_angle),
		sf2_member(follow_time)
	)

	void Simple_ai_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		attack_distance=Distance(s.attack_distance);
		near=Distance(s.near);
		max=Distance(s.max);
		near_angle=s.near_angle * 1_deg;
		far_angle=s.far_angle* 1_deg;
		_follow_time=Time(s.follow_time);
	}

	void Simple_ai_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}


	Simple_ai_comp::Simple_ai_comp(ecs::Entity& owner)
	    : Component(owner), attack_distance(2_m),
	      near(2_m), max(10_m), near_angle(360_deg), far_angle(180_deg),
	      _follow_time(0.5_s), _follow_time_left(0) {

		auto controller_m = owner.get<controller::Controllable_comp>();

		if(controller_m.is_nothing())
			owner.emplace<controller::Controllable_comp>(this);

		else
			controller_m.get_or_throw().controller = this;

	}

	void Simple_ai_comp::no_target(Time dt)noexcept {
		if(_target) {
			_follow_time_left-=dt;
			if(_follow_time_left<=0_s)
				_target.reset();
		}
	}

	void Simple_ai_comp::operator()(controller::Controllable_interface& c) {
		if(_target) {
			owner().get<physics::Transform_comp>().process([&](auto& t){
				auto tt_m = _target->get<physics::Transform_comp>();
				if(tt_m.is_nothing()) {
					_target.reset();
					return;
				}

				auto& tt = tt_m.get_or_throw();
				auto dir = remove_units(tt.position() - t.position());

				auto distance = glm::length(dir) * 1_m;

				c.look_at(remove_units(tt.position()));

				if(distance<=attack_distance)
					c.attack();

				else
					c.move(dir);
			});
		}
	}

}
}
}