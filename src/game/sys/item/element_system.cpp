#include "element_system.hpp"

#include "element_comp.hpp"

#include <vector>

namespace mo {
namespace sys {
namespace item {

	namespace {

	}

	struct Element_system::Config {
		float per_use = 0.1f;
	};

	Element_system::Element_system(
	        asset::Asset_manager& assets,
	        ecs::Entity_manager& entity_manager,
	        combat::Weapon_modifier_collection& collection)
	    : combat::Weapon_modifier(collection, 50), _config(std::make_unique<Config>()) {

		entity_manager.register_component_type<Element_comp>();

		// TODO: load _weapons
	}
	Element_system::~Element_system() = default;

	void Element_system::process(ecs::Entity& e, combat::Weapon& w) {
		e.get<Element_comp>().process([&](Element_comp& elem) {
			std::vector<level::Element> elements;
			elements.reserve(element_slots);

			for(auto i : util::range(element_slots)) {
				auto& slot = elem.slot(i);
				if(slot.active) {
					elements.push_back(slot.element);
				}
			}

			// TODO: lookup elements and mod w

		});
	}

	void Element_system::on_attack(ecs::Entity& e, const combat::Weapon&) {
		e.get<Element_comp>().process([&](Element_comp& elem) {
			elem.mod_slots_fill(-_config->per_use);
		});
	}

}
}
}
