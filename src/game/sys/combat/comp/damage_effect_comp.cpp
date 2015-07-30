#define MO_BUILD_SERIALIZER
#include "damage_effect_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	struct Damage_effect_comp::Persisted_state {
		Damage_effect type;
		float left;
		float confusion;

		Persisted_state(const Damage_effect_comp& c)
		    : type(c._type),
		      left(c._time_left / 1_s),
		      confusion(c._confusion){}
	};

	sf2_structDef(Damage_effect_comp::Persisted_state,
		sf2_member(type),
		sf2_member(left),
		sf2_member(confusion)
	)

	void Damage_effect_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_type = s.type;
		_time_left = s.left * 1_s;
		_confusion = s.confusion;
	}
	void Damage_effect_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
}
}
