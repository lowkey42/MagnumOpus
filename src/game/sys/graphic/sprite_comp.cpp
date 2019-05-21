#include "sprite_comp.hpp"

#include <sf2/sf2.hpp>

#include <string>

namespace mo {
namespace sys {
namespace graphic {

	void Sprite_comp::load(sf2::JsonDeserializer& state,
	                       asset::Asset_manager& assets){
		std::string aid = _animation ? _animation.aid().str() : "";

		state.read_virtual(
			sf2::vmember("aid", aid)
		);

		_animation = assets.load<renderer::Animation>(asset::AID(aid));
	}

	void Sprite_comp::save(sf2::JsonSerializer& state)const {
		std::string aid = _animation ? _animation.aid().str() : "";

		state.write_virtual(
			sf2::vmember("aid", aid)
		);
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
