#include "state_system.hpp"


namespace mo {
namespace sys {
namespace state {

	State_system::State_system(ecs::Entity_manager& em)
		: _states(em.list<State_comp>()) {

		em.register_component_type<State_comp>();
	}

	void State_system::update(Time dt) {
		for(auto& state : _states) {
			auto state_update = state.update(dt);
			state_update.process([&](const auto& ns){
				this->state_change_events.inform(state.owner(), ns);
			});;
		}
	}

}
}
}
