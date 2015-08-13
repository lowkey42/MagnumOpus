#include "state_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace state {

	using namespace unit_literals;

	struct State_comp::Persisted_state {
		Entity_state state, state_background;
		float state_left, state_bg_left;
		bool delete_dead;

		Persisted_state(const State_comp& c)
			: state(c._state_primary.s),
		      state_background(c._state_background.s),
		      state_left(c._state_primary.left /1_s),
		      state_bg_left(c._state_background.left/1_s),
		      delete_dead(c._delete_dead) {}
	};

	sf2_enumDef(Entity_state,
		sf2_value(idle),
		sf2_value(walking),
		sf2_value(attacking_melee),
		sf2_value(attacking_range),
		sf2_value(interacting),
		sf2_value(taking),
		sf2_value(change_weapon),
		sf2_value(damaged),
		sf2_value(healed),
		sf2_value(dying),
		sf2_value(dead),
		sf2_value(resurrected)
	)

	sf2_structDef(State_comp::Persisted_state,
		sf2_member(state),
		sf2_member(state_background),
		sf2_member(state_left),
		sf2_member(state_bg_left),
		sf2_member(delete_dead)
	)

	void State_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_state_primary = State_data{s.state, 1.0f, s.state_left*1_s};
		_state_background = {s.state_background, 1.0f, s.state_bg_left*1_s};
		_delete_dead = s.delete_dead;
	}
	void State_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

	namespace {
		Time required_time_for(Entity_state state)noexcept {
			switch(state) {
				case Entity_state::idle:			return  0_s;
				case Entity_state::walking:			return  0.1_s;
				case Entity_state::attacking_melee:	return  0.1_s;
				case Entity_state::attacking_range:	return  0.1_s;
				case Entity_state::interacting:		return  0.1_s;
				case Entity_state::taking:			return  0.5_s;
				case Entity_state::change_weapon:	return  0.5_s;
				case Entity_state::damaged:			return  0.3_s;
				case Entity_state::healed:			return  0.1_s;
				case Entity_state::dead:			return  0_s;
				case Entity_state::dying:			return  0.05_s;
				case Entity_state::resurrected:		return  0.1_s;
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
				case Entity_state::dead:			return true;
				case Entity_state::dying:			return false;
				case Entity_state::resurrected:		return false;
			}

			INVARIANT(false, "Unexpected state "<<static_cast<int>(state));
		}
		int priority(Entity_state state)noexcept {
			switch(state) {
				case Entity_state::idle:			return 0;
				case Entity_state::walking:			return 1;
				case Entity_state::attacking_melee:	return 3;
				case Entity_state::attacking_range:	return 3;
				case Entity_state::interacting:		return 2;
				case Entity_state::taking:			return 2;
				case Entity_state::change_weapon:	return 3;
				case Entity_state::damaged:			return 4;
				case Entity_state::healed:			return 2;
				case Entity_state::dead:			return 10;
				case Entity_state::dying:			return 10;
				case Entity_state::resurrected:		return 5;
			}

			INVARIANT(false, "Unexpected state "<<static_cast<int>(state));
		}

		Entity_state get_next(Entity_state state)noexcept {
			switch(state) {
				case Entity_state::dying:
				case Entity_state::dead:
					return Entity_state::dead;

				default:
					return Entity_state::idle;
			}
		}
	}

	void State_comp::state(Entity_state s, float magnitude)noexcept {
		INVARIANT(magnitude>=0 && magnitude<=1, "magnitude is out of range: "+util::to_string(magnitude));

		auto& cstate = _state_primary.left>0_s ? _state_primary : _state_background;

		if(priority(cstate.s)>priority(s) && cstate.left>0_s)
			return;

		auto& state = is_background(s) ? _state_background : _state_primary;

		state.s=s;
		state.magnitude=magnitude;
		state.left=required_time_for(s);
	}

	auto State_comp::update(Time dt)noexcept -> util::maybe<State_data&> {
		if(_state_background.left>0_s) {
			_state_background.left-=dt;

			// time is up, lets idle
			if(_state_background.left<=0_s) {
				state(get_next(_state_background.s));
			}
		}

		if(_state_primary.left>0_s) {
			_state_primary.left-=dt;
			if(_state_primary.left<=0_s) {
				state(get_next(_state_primary.s));
			}
		}


		auto& cstate = _state_primary.left>0_s ? _state_primary : _state_background;

		if(cstate!=_state_last) {
			_state_last = cstate;
			return util::justPtr(&cstate);
		}

		return util::nothing();
	}

}
}
}
