#define MO_BUILD_SERIALIZER

#include "health_comp.hpp"

#include "../../../level/elements.hpp"

namespace mo {
namespace sys {
namespace combat {


	void Health_comp::load(sf2::JsonDeserializer& state,
	                       asset::Asset_manager&) {
		std::vector<level::Element> resistences;
		std::vector<level::Element> vulnerabilities;
		for(auto e : _resistences)
			resistences.push_back(e);

		for(auto e : _vulnerabilities)
			vulnerabilities.push_back(e);


		state.read_virtual(
			sf2::vmember("auto_heal_max", _auto_heal_max),
			sf2::vmember("auto_heal", _auto_heal),
			sf2::vmember("max_hp", _max_hp),
			sf2::vmember("current_hp", _current_hp),
			sf2::vmember("physical_resistence", _physical_resistence),
			sf2::vmember("resistences", resistences),
			sf2::vmember("vulnerabilities", vulnerabilities),
			sf2::vmember("death_effect", _death_effect),
			sf2::vmember("death_force_feedback", _death_force_feedback)
		);



		if(_current_hp==0)
			_current_hp = _max_hp;

		_resistences = level::Elements{resistences};
		_vulnerabilities = level::Elements{vulnerabilities};

	}
	void Health_comp::save(sf2::JsonSerializer& state)const {
		std::vector<level::Element> resistences;
		std::vector<level::Element> vulnerabilities;
		for(auto e : _resistences)
			resistences.push_back(e);

		for(auto e : _vulnerabilities)
			vulnerabilities.push_back(e);


		state.write_virtual(
			sf2::vmember("auto_heal_max", _auto_heal_max),
			sf2::vmember("auto_heal", _auto_heal),
			sf2::vmember("max_hp", _max_hp),
			sf2::vmember("current_hp", _current_hp),
			sf2::vmember("physical_resistence", _physical_resistence),
			sf2::vmember("resistences", resistences),
			sf2::vmember("vulnerabilities", vulnerabilities),
			sf2::vmember("death_effect", _death_effect),
			sf2::vmember("death_force_feedback", _death_force_feedback)
		);
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
