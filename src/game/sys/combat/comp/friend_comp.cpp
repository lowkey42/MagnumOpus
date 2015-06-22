#include "friend_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	struct Friend_comp::Persisted_state {
		int group;

		Persisted_state(const Friend_comp& c)
				: group(c._group) {}
	};

	sf2_structDef(Friend_comp::Persisted_state,
		sf2_member(group)
	)

	void Friend_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_group = s.group;
	}
	void Friend_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
