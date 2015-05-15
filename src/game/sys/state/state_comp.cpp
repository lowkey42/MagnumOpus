#include "state_comp.hpp"


namespace mo {
namespace sys {
namespace state {

	using namespace unit_literals;

	namespace {
		Time required_time_for(Entity_state state)noexcept {
			switch(state) {
				case Entity_state::idle:			return  0_s;
				case Entity_state::walking:			return  0.1_s;
				case Entity_state::attacking_melee:	return  0.25_s;
				case Entity_state::attacking_range:	return  0.25_s;
				case Entity_state::interacting:		return  1_s;
				case Entity_state::taking:			return  1.0_s;
				case Entity_state::change_weapon:	return  0.1_s;
				case Entity_state::damaged:			return  0.5_s;
				case Entity_state::healed:			return  0.5_s;
				case Entity_state::died:			return  1.0_s;
				case Entity_state::resurrected:		return  1.0_s;
			}

			INVARIANT(false, "Unexpected state "<<static_cast<int>(state));
		}
		bool is_background(Entity_state state)noexcept {
			switch(state) {
				case Entity_state::idle:			return true;
				case Entity_state::walking:			return true;
				case Entity_state::attacking_melee:	return false;
				case Entity_state::attacking_range:	return false;
				case Entity_state::interacting:		return false;
				case Entity_state::taking:			return true;
				case Entity_state::change_weapon:	return true;
				case Entity_state::damaged:			return false;
				case Entity_state::healed:			return false;
				case Entity_state::died:			return false;
				case Entity_state::resurrected:		return false;
			}

			INVARIANT(false, "Unexpected state "<<static_cast<int>(state));
		}
	}

	void State_comp::state(Entity_state s, float magnitude)noexcept {
		auto& state = is_background(s) ? _state_background : _state_primary;

		state.s=s;
		state.magnitude=magnitude;
		state.left=required_time_for(s);
	}

	auto State_comp::update(Time dt)noexcept -> util::maybe<State_data> {
		if(_state_background.left>0_s) {
			_state_background.left-=dt;

			// time is up, lets idle
			if(_state_background.left<=0_s) {
				_state_background.s = Entity_state::idle;
				_state_background.left = 0_s;
				_state_background.magnitude = 1.f;
			}
		}

		if(_state_primary.left>0_s) {
			_state_primary.left-=dt;
		}


		auto& cstate = _state_primary.left>0_s ? _state_primary : _state_background;

		if(cstate!=_state_last) {
			// TODO[foe]: call listener
			_state_last = cstate;
			return util::justCopy(cstate);
		}

		return util::nothing();
	}

}
}
}
