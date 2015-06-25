#include "score_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	struct Score_comp::Persisted_state {
		int value;

		Persisted_state(const Score_comp& c)
			: value(c._value) {}
	};

	sf2_structDef(Score_comp::Persisted_state,
		sf2_member(value)
	)

	void Score_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_value = s.value;
	}
	void Score_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
