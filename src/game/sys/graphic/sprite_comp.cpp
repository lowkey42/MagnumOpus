#include "sprite_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

#include <string>

namespace mo {
namespace sys {
namespace graphic {

	struct Sprite_comp::Persisted_state {
		std::string aid;

		Persisted_state(const Sprite_comp& c)
		    : aid(c._animation.aid().str()){}

	};

	sf2_structDef(Sprite_comp::Persisted_state,
		sf2_member(aid)
	)

	void Sprite_comp::load(ecs::Entity_state &state){
		auto s = state.read_to(Persisted_state{*this});
		_animation = state.asset_mgr().load<renderer::Animation>(asset::AID(s.aid));
	}

	void Sprite_comp::store(ecs::Entity_state &state){
		state.write_from(Persisted_state{*this});
	}

	renderer::Sprite_batch::Sprite Sprite_comp::sprite() const noexcept{
		return renderer::Sprite_batch::Sprite{{}, 0, 0, _animation->uv(_current_frame, _anim_type), &*_animation->texture()};
	}

	void Sprite_comp::animation_type(renderer::Animation_type type) noexcept {

		renderer::Animation_type before = _anim_type;

		if(animation()->animation_exists(type)){
			_anim_type = type;
		} else {
			_anim_type = renderer::Animation_type::idle;
		}

		if(before != _anim_type)
			_current_frame = 0;
	}

}
}
}
