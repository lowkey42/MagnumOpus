#include "sprite_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

#include <string>

namespace game {
namespace sys {
namespace sprite {

	struct Sprite_comp::Persisted_state {
		std::string aid;
		float uv_x;
		float uv_y;
		float uv_z;
		float uv_w;
		Persisted_state(const Sprite_comp& c) :
		    aid(c._texture.str()), uv_x(c._uv.x), uv_y(c._uv.y), uv_z(c._uv.z), uv_w(c._uv.w){}

	};

	sf2_structDef(Sprite_comp::Persisted_state,
		sf2_member(aid),
	    sf2_member(uv_x),
	    sf2_member(uv_y),
	    sf2_member(uv_z),
	    sf2_member(uv_w)
	)

	void Sprite_comp::load(core::ecs::Entity_state &state){
		auto s = state.read_to(Persisted_state{*this});
		_texture = core::asset::AID(s.aid);
		_uv = glm::vec4(s.uv_x, s.uv_y, s.uv_z, s.uv_w);
	}

	void Sprite_comp::store(core::ecs::Entity_state &state){
		state.write_from(Persisted_state{*this});
	}


}
}
}
