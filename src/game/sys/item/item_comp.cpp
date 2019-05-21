#define MO_BUILD_SERIALIZER
#include "item_comp.hpp"

#include <core/ecs/serializer_impl.hpp>


namespace mo {
namespace sys {
namespace item {

	struct Item_comp::Persisted_state {
		Item_target    target;
		level::Element element;
		float          mod;
		bool           joinable;

		Persisted_state(const Item_comp& c)
			: target(c._target),
			  element(c._element),
			  mod(c._mod),
		      joinable(c._joinable){}
	};

	sf2_enumDef(Item_target,
		sf2_value(health),
		sf2_value(score),
		sf2_value(element),
		sf2_value(bullet_time)
	)
	sf2_structDef(Item_comp::Persisted_state,
		sf2_member(target),
		sf2_member(element),
		sf2_member(mod),
		sf2_member(joinable)
	)

	void Item_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_target   = s.target;
		_element  = s.element;
		_mod      = s.mod;
		_joinable = s.joinable;
	}
	void Item_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
