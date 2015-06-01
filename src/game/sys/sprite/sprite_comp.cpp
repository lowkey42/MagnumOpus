#include "sprite_comp.hpp"

#include <sf2/sf2.hpp>
#include "../../../core/ecs/serializer_impl.hpp"

#include <string>

namespace mo {
namespace sys {
namespace sprite {

	struct Sprite_comp::Persisted_state {
		std::string aid;
		Persisted_state(const Sprite_comp& c) :
			aid(c._animation.aid().str()){}

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

		// resetting currentFrame to 0 after animation has been changed to
		// another one so if the old animation and had more frames than the
		// new there will be no blank sprite till next reset in calculation
		if(before != _anim_type)
			_current_frame = 0;
	}

}
}
}
