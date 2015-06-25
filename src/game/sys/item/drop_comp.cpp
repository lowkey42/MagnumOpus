#include "drop_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace item {


	struct Drop_comp::Persisted_state {
		uint8_t group;

		Persisted_state(const Drop_comp& c)
		    : group(c._group) {}
	};

	sf2_structDef(Drop_comp::Persisted_state,
		sf2_member(group)
	)

	void Drop_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_group      = s.group;
	}
	void Drop_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
