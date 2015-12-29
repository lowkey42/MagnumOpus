#define MO_BUILD_SERIALIZER
#include "element_system.hpp"

#include "element_comp.hpp"
#include "../combat/comp/weapon_comp.hpp"

#include <core/asset/asset_manager.hpp>
#include <sf2/sf2.hpp>

#include <vector>


namespace mo {
namespace sys {
namespace item {

	using namespace level;

	namespace {

		struct Config_data {
			std::map<std::vector<Element>, combat::Weapon> weapons;
		};
		sf2_structDef(Config_data,
			weapons
		)
	}

	using Reaction_table = std::unordered_map<Elements, combat::Weapon>;

	struct Element_system::Config {
		Reaction_table weapons;
	};

}
}
}

namespace mo {
namespace asset {
	using Type = sys::item::Element_system::Config;

	template<>
	struct Loader<Type> {
		using RT = std::shared_ptr<Type>;

		static RT load(istream in) throw(Loading_failed) {
			auto r = std::make_shared<Type>();

			sys::item::Config_data data;
			sf2::deserialize_json(in, [&](auto& msg, uint32_t row, uint32_t column) {
				ERROR("Error parsing JSON from "<<in.aid().str()<<" at "<<row<<":"<<column<<": "<<msg);
			}, data);

			for(auto& rec : data.weapons) {
				r->weapons.emplace(rec.first, rec.second);
			}

			return r;
		}

		static void store(ostream out, const Type& asset) throw(Loading_failed) {
			//sf2::writeStream(out,asset);
			INVARIANT(false, "NOT IMPLEMENTED");
		}
	};
}
}

namespace mo {
namespace sys {
namespace item {

	Element_system::Element_system(
	        asset::Asset_manager& assets,
	        ecs::Entity_manager& entity_manager,
	        combat::Weapon_modifier_collection& collection)
	    : combat::Weapon_modifier(collection, 50) {

		entity_manager.register_component_type<Element_comp>();

		_config = assets.load<Element_system::Config>("cfg:element_interactions"_aid);
	}
	Element_system::~Element_system() = default;

	namespace {
		auto lookup_reaction(Elements elements,
		                     const Reaction_table& reactions) -> util::maybe<const combat::Weapon&> {
			if(!elements)
				return util::nothing();

			auto rec = reactions.find(elements);
			if(rec!=reactions.end()) {
				return util::just(rec->second);

			} else {
				for(auto e : elements) {
					auto r = lookup_reaction(elements.without(e), reactions);
					if(r.is_some())
						return r;
				}
			}

			return util::nothing();
		}
	}

	void Element_system::process(ecs::Entity& e, combat::Weapon& w) {
		e.get<Element_comp>().process([&](Element_comp& elem) {
			std::vector<level::Element> elements_vec;
			elements_vec.reserve(element_slots);

			for(auto i : util::range(element_slots)) {
				auto& slot = elem.slot(i);
				if(slot.active) {
					elements_vec.push_back(slot.element);
				}
			}
			const auto& reactions = _config->weapons;

			auto found_weapon = lookup_reaction(elements_vec, reactions);
			found_weapon.process([&w](auto& nw){
				w = nw;
			});

			if(!elements_vec.empty() && found_weapon.is_nothing()) {
				auto& msg = ::mo::util::debug(__func__, __FILE__, __LINE__);
				msg<<"No reaction for: ";
				for(auto e : elements_vec)
					msg<<e<<", ";

				msg<<std::endl;
			}

		});
	}

	void Element_system::on_attack(ecs::Entity& e, const combat::Weapon& w) {
		e.get<Element_comp>().process([&](Element_comp& elem) {
			elem.mod_slots_fill(-w.fuel_usage);
		});
	}

}
}
}
