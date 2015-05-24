#include "animation.hpp"

#include <sf2/sf2.hpp>

namespace mo{
namespace renderer{

	struct Animation_frame_data{
		int row;
		float fps;
		int frames;
	};

	struct Animation_data{

		Animation_data(){}
		~Animation_data() = default;

		int frame_width;
		int frame_height;
		Texture_ptr texture;
		std::string texName;
		Animation_type currentAnim = Animation_type::idle;
		std::unordered_map<Animation_type, Animation_frame_data> animations;
	};

	sf2_enumDef(Animation_type,
		sf2_value(idle),
		sf2_value(walking),
		sf2_value(attacking_melee),
		sf2_value(attacking_range),
		sf2_value(interacting),
		sf2_value(taking),
		sf2_value(change_weapon),
		sf2_value(damaged),
		sf2_value(healed),
		sf2_value(died),
		sf2_value(resurrected)
	)

	sf2_structDef(Animation_frame_data,
		sf2_member(row),
		sf2_member(fps),
		sf2_member(frames)
	)

	sf2_structDef(Animation,
		sf2_member(_data)
	)

	sf2_structDef(Animation_data,
		sf2_member(frame_width),
		sf2_member(frame_height),
		sf2_member(texName),
		sf2_member(animations),
	)

	Animation::Animation(std::unique_ptr<Animation_data> data){
		_data = std::move(data);
	}

	Animation::~Animation() = default;

	Animation& Animation::operator=(Animation&& rhs) noexcept {
		_data = std::move(rhs._data);
		return *this;
	}

	bool Animation::animation_exists(Animation_type type) const noexcept {
		if(_data->animations.find(type) == _data->animations.end()){
			return false;
		}
		return true;
	}

	int Animation::frame_width() const noexcept{
		return _data->frame_width;
	}

	int Animation::frame_height() const noexcept{
		return _data->frame_height;
	}

	Texture_ptr Animation::texture() const noexcept{
		return _data->texture;
	}

	// Converting float frame to int frame --> 0.9 = 0 or 1.3 = 1
	glm::vec4 Animation::uv(int frame, Animation_type type) const noexcept{

		// Calculating corresponding uv-coords
		// uv-coords -> 1: x = xStart from left | 2: y = yStart from down | 3: z = xEnd from left | 4: w = yEnd from down

		_data->currentAnim = type;
		int row = _data->animations.find(type) -> second.row;

		float width = _data->frame_width / static_cast<float>(_data->texture->width());
		float height = _data->frame_height / static_cast<float>(_data->texture->height());
		float startX = frame * width;
		float startY = 1 - height - (row * height);
		const glm::vec4 uv = glm::vec4(startX, startY, startX + width, startY + height);

		return uv;
	}

	float Animation::next_frame(Animation_type type, float cur_frame, float deltaTime, bool repeat) const noexcept{
		int max_frames = _data->animations.find(type)->second.frames;
		int fps = _data->animations.find(type)->second.fps;
		float ret = cur_frame;

		/*if(ret > max_frames){
			ret = 0;
			DEBUG("ERROR!");
			DEBUG("next_frame: " << ret << " / max Frames: " << max_frames);
		}*/

		// checking if cur_frame + change is in max_frames bounding for cur Animation
		// if not but repeat is set -> set Animation back to 0 else keep last frame

		if(cur_frame + fps * deltaTime < max_frames){
			ret = cur_frame + fps * deltaTime;
		} else if(repeat){
			ret = 0;
		}

		return ret;

	}

}

namespace asset {

	std::shared_ptr<renderer::Animation> Loader<renderer::Animation>::load(istream in) throw(Loading_failed){
		auto r = std::make_unique<renderer::Animation_data>();

		std::string data = in.content();
		sf2::io::StringCharSource source(data);

		sf2::ParserDefChooser<renderer::Animation_data>::get().parse(source, *r.get());

		r->texture = in.manager().load<renderer::Texture>(r->texName);

		// Generating new Animation-Shared-Ptr and set _data-ptr to what r pointed to
		auto anim = std::make_shared<renderer::Animation>(std::move(r));

		return anim;
	}

	void Loader<renderer::Animation>::store(ostream out, renderer::Animation& asset) throw(Loading_failed) {
		asset._data->texName = asset._data->texture.aid().name();
		std::string data = sf2::writeString(asset);
		out.write(data.c_str(), data.length());
	}
}
}
