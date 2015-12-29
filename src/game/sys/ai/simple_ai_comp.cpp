#include "simple_ai_comp.hpp"

#include "../controller/controllable_comp.hpp"
#include "../physics/transform_comp.hpp"

#include "../../../core/units.hpp"

#include <sf2/sf2.hpp>

#include <core/utils/random.hpp>
#include "../../level/level.hpp"

namespace mo {
namespace sys {
namespace ai {

	using namespace unit_literals;


	void Simple_ai_comp::load(sf2::JsonDeserializer& state,
	                          asset::Asset_manager&) {
		float attack_distance_f = attack_distance / 1_m;
		float near_f = near / 1_m;
		float max_f = max / 1_m;
		float near_angle_f = near_angle / 1_deg;
		float far_angle_f = far_angle / 1_deg;
		float follow_time_f = _follow_time / 1_s;

		state.read_virtual(
			sf2::vmember("attack_distance", attack_distance_f),
			sf2::vmember("near", near_f),
			sf2::vmember("max", max_f),
			sf2::vmember("near_angle", near_angle_f),
			sf2::vmember("far_angle", far_angle_f),
			sf2::vmember("follow_time", follow_time_f),
			sf2::vmember("swarm_id", _swarm_id)
		);

		attack_distance = attack_distance_f * 1_m;
		near = near_f * 1_m;
		max = max_f * 1_m;
		near_angle = near_angle_f * 1_deg;
		far_angle = far_angle_f * 1_deg;
		_follow_time = follow_time_f * 1_s;
	}

	void Simple_ai_comp::save(sf2::JsonSerializer& state)const {
		state.write_virtual(
			sf2::vmember("attack_distance", attack_distance / 1_m),
			sf2::vmember("near", near / 1_m),
			sf2::vmember("max", max / 1_m),
			sf2::vmember("near_angle", near_angle / 1_deg),
			sf2::vmember("far_angle", far_angle / 1_deg),
			sf2::vmember("follow_time", _follow_time / 1_s),
			sf2::vmember("swarm_id", _swarm_id)
		);
	}


	namespace {
		auto rng = util::create_random_generator();

		Angle random_angle(Angle min, Angle max) {
			return Angle{util::random_real(rng, remove_unit(min), remove_unit(max))};
		}
	}

	Simple_ai_comp::Simple_ai_comp(ecs::Entity& owner)
	    : Component(owner), attack_distance(2_m),
	      near(2_m), max(10_m), near_angle(360_deg), far_angle(180_deg),
	      _follow_time(0.5_s), _follow_time_left(0),
	      _wander_dir(static_cast<float>(util::random_int(rng, 0,4))*90_deg), _rot_delay(0_s) {

		auto controller_m = owner.get<controller::Controllable_comp>();

		if(controller_m.is_nothing())
			owner.emplace<controller::Controllable_comp>(type());

		else
			controller_m.get_or_throw().set(type());
	}

	void Simple_ai_comp::no_target(Time dt, level::Level& level)noexcept {
		if(_target) {
			_follow_time_left-=dt;
			if(_follow_time_left<=0_s)
				_target.reset();

		} else {
			auto pos = owner().get<physics::Transform_comp>()
			            .process(Position{0,0}, [&](auto& t){return t.position();});

			auto is_solid = [&](Angle a, float o){
				auto dest = pos+rotate(Position{1_m*o,0_m}, a);
				return level.solid(dest.x.value(),dest.y.value());
			};

			_rot_delay+=dt;
			if(_rot_delay>2_s) {
				_wander_dir+=random_angle(-20_deg, 20_deg);
				_rot_delay = 0_s;
			}

			if(is_solid(_wander_dir,1) || is_solid(_wander_dir, 2.f)) {

				Angle a = util::random_bool(rng, 0.5) ? -90_deg : 90_deg;
				_rot_delay = 0_s;

				do {

					if(!is_solid(_wander_dir+a,1)) {
						_wander_dir+=a;
						break;

					} else if(!is_solid(_wander_dir-a,1)) {

						_wander_dir-=a;
						break;

					} else {
						a*=2;
					}
				} while(abs(a)<=180_deg);
			}
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

				if(distance>attack_distance/2)
					c.move(dir);
			});

		} else {
			owner().get<physics::Transform_comp>().process([&](auto& t){
				c.move(rotate(glm::vec2(1,0), t.rotation()));
				c.look_in_dir(rotate(glm::vec2(1,0), _wander_dir));
			});
		}
	}

}
}
}
