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
				if(e._self_effects[i]!=level::Element::neutral)
					self.push_back(e._self_effects[i]);

			slots.reserve(e._slots.size());
			for(auto& es : e._slots)
				if(es.element!=level::Element::neutral && es.fill>0)
					slots.push_back(es);
		}
	};

	sf2_structDef(Element_slot,
		sf2_member(element),
		sf2_member(fill),
		sf2_member(active),
	)
	sf2_structDef(Element_comp::Persisted_state,
		//sf2_member(self), // FIXME[foe]: crashes on load
		sf2_member(slots)
	)

	void Element_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_self_effects = Elements{s.self};

		INVARIANT(s.slots.size()<=element_slots, "loaded more slots than available");

		auto i=0;
		for(auto& es : s.slots)
			_slots[i++] = es;
	}
	void Element_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

	bool Element_comp::flip_slot(std::size_t i)noexcept {
		if(i>=element_slots)
			return false;

		auto& s = _slots[i];
		if(s.fill<=0)
			return false;

		s.active=!s.active;
		return true;
	}
	bool Element_comp::mod_slot_fill(std::size_t i, float mod)noexcept {
		auto& s = _slots.at(i);

		if(s.fill<=0)
			return false;

		s.fill = glm::clamp(s.fill + mod, 0.f, 1.f);
		return true;
	}
	bool Element_comp::add_slot(level::Element e, float fill)noexcept {
		for(auto& s : _slots) {
			if((s.element==e || s.fill<=0) && s.fill<1.f) {
				s.element = e;
				s.fill = glm::clamp(s.fill+fill, 0.f, 1.f);
				return true;
			}
		}

		return false;
	}
}
}
}
