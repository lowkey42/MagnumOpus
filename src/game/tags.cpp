#include "tags.hpp"

#include <sf2/sf2.hpp>
#include <core/ecs/serializer_impl.hpp>

namespace mo {

	struct Player_tag_comp::Persisted_state {
		uint8_t id;

		Persisted_state(const Player_tag_comp& c) : id(c._id) {

		}
	};

	sf2_structDef(Player_tag_comp::Persisted_state, sf2_member(id))

	void Player_tag_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		_id = s.id;
	}
	void Player_tag_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

}
