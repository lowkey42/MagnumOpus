#include "collector_comp.hpp"

#include <core/ecs/serializer_impl.hpp>

#include "../physics/physics_comp.hpp"
#include "../physics/transform_system.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace unit_literals;

	struct Collector_comp::Persisted_state {
		int group;

		Persisted_state(const Collector_comp& c)
				: group(c._group) {}
	};

	sf2_structDef(Collector_comp::Persisted_state,
		sf2_member(group)
	)

	void Collector_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_group = s.group;
	}
	void Collector_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

	void Collector_comp::take(physics::Transform_system& ts) {
		owner().get<physics::Transform_comp>().process([&](auto& t){

			ts.foreach_in_range(t.position(), t.rotation(), 0.5_m, 10_m, 60_deg, 90_deg,
			                    [&](ecs::Entity& e){
				util::process(e.get<physics::Transform_comp>(),
				              e.get<physics::Physics_comp>())
				>> [&](auto& tt, auto& tp){
					tp.apply_force(500_N * remove_units(t.position()-tt.position()) );
				};
			});

		});
	}

}
}
}
