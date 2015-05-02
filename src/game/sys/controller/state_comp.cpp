#include "state_comp.hpp"


namespace mo {
namespace sys {
namespace controller {

	using namespace unit_literals;

	void State_comp::state(Entity_state s)noexcept {
		if(_time_left<0_s || _time_left>=0.5_s)
			_last_state = _current_state;

		_current_state = s;
		_time_left = _required_time_for(s);
	}

	void State_comp::update(Time dt)noexcept {
		if(_time_left>=0_s) {
			_time_left-=dt;

			if(_time_left<=0_s) {
				_current_state = _last_state;
				_last_state = Entity_state::idle;
			}
		}
	}

	Time State_comp::_required_time_for(Entity_state state)const noexcept {
		switch(state) {
			case Entity_state::idle:			return -1_s;
			case Entity_state::walking:			return  1_s;
			case Entity_state::attacking_melee:	return  1_s;
			case Entity_state::attacking_range:	return  1.5_s;
			case Entity_state::interacting:		return  2_s;
			case Entity_state::taking:			return  1.5_s;
			case Entity_state::change_weapon:	return  0.5_s;
			case Entity_state::damaged:			return  0.5_s;
			case Entity_state::healed:			return  0.5_s;
			case Entity_state::died:			return  2.0_s;
			case Entity_state::resurrected:		return  2.0_s;
		}

		INVARIANT(false, "Unexpected state "<<static_cast<int>(state));
	}

}
}
}
