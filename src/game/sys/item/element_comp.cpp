#define MO_BUILD_SERIALIZER
#include "element_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace item {

	using namespace level;
	using namespace util;

	struct Element_comp::Persisted_state {
		std::vector<level::Element> self;
		std::vector<Element_slot> slots;

		Persisted_state(Element_comp& e) {
			auto self_size = e._self_effects.size();
			self.reserve(self_size);
			for(auto i : range(self_size))
				self.push_back(e._self_effects[i]);

			slots.reserve(e._slots.size());
			for(auto& es : e._slots)
				slots.push_back(es);
		}
	};

	sf2_structDef(Element_slot,
		sf2_member(element),
		sf2_member(fill),
		sf2_member(active),
	)
	sf2_structDef(Element_comp::Persisted_state,
		sf2_member(self),
		sf2_member(slots)
	)

	void Element_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		// TODO
	}
	void Element_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});

	}

}
}
}
