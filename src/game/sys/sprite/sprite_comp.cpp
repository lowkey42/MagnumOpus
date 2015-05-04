#include "sprite_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

#include <string>

namespace mo {
namespace sys {
namespace sprite {

	struct Sprite_comp::Persisted_state {
		std::string aid;
		// add Animationdata to save
		Persisted_state(const Sprite_comp& c) :
			aid(c._texture.aid().str()){}

    };

	sf2_structDef(Sprite_comp::Persisted_state,
		sf2_member(aid)
	)

	void Sprite_comp::load(ecs::Entity_state &state){
		auto s = state.read_to(Persisted_state{*this});
		// TODO _texture = asset::AID(s.aid);
	}

	void Sprite_comp::store(ecs::Entity_state &state){
		state.write_from(Persisted_state{*this});
	}


}
}
}
