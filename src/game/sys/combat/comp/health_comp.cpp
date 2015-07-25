#define MO_BUILD_SERIALIZER

#include "health_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

#include "../../../level/elements.hpp"

namespace mo {
namespace sys {
namespace combat {

	struct Health_comp::Persisted_state {
		float auto_heal_max;
		float auto_heal;
		float max_hp;
		float current_hp;

		float physical_resistence;
		std::vector<level::Element> resistences;
		std::vector<level::Element> vulnerabilities;

		Persisted_state(const Health_comp& c)
		    : auto_heal_max(c._auto_heal_max), auto_heal(c._auto_heal),
		      max_hp(c._max_hp), current_hp(c._current_hp),
		      physical_resistence(c._physical_resistence) {
			for(auto e : c._resistences)
				resistences.push_back(e);

			for(auto e : c._vulnerabilities)
				vulnerabilities.push_back(e);
		}
	};

	sf2_structDef(Health_comp::Persisted_state,
		sf2_member(auto_heal_max),
		sf2_member(auto_heal),
		sf2_member(max_hp),
		sf2_member(current_hp),
		sf2_member(physical_resistence),
		sf2_member(resistences),
		sf2_member(vulnerabilities)
	)

	void Health_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_auto_heal_max = s.auto_heal_max;
		_auto_heal = s.auto_heal;
		_max_hp = s.max_hp;
		_current_hp = s.current_hp==0 ? s.max_hp : s.current_hp;
		_physical_resistence = s.physical_resistence;

		_resistences = level::Elements{s.resistences};
		_vulnerabilities = level::Elements{s.vulnerabilities};
	}
	void Health_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

	void Health_comp::damage(float hp, level::Element type)noexcept{
		if(type==level::Element::neutral)
			hp *= (1.f - _physical_resistence);

		else if(_resistences | type)
			hp *= 0.25f;

		else if(_vulnerabilities | type)
			hp *= 4.f;

		_damage+=hp;
	}

}
}
}
