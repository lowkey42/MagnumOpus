#include "animation.hpp"

#include <sf2/sf2.hpp>

namespace mo{
namespace renderer{

	using namespace unit_literals;

	struct Animation_frame_data{
		int row;
		float fps;
		int frames;
		float modulation = 1.0f;
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
		idle,
		walking,
		attacking_melee,
		attacking_range,
		interacting,
		taking,
		change_weapon,
		damaged,
		healed,
		died,
		dying,
		resurrected
	)

	sf2_structDef(Animation_frame_data,
		row,
		fps,
		frames,
		modulation
	)

	sf2_structDef(Animation,
		_data
	)

	sf2_structDef(Animation_data,
		frame_width,
		frame_height,
		texName,
		animations
	)

	Animation::Animation(std::unique_ptr<Animation_data> data){
		_data = std::move(data);
	}

	Animation::~Animation() = default;

	Animation& Animation::operator=(Animation&& rhs) noexcept {
		_data = std::move(rhs._data);
		return *this;
	}

	void Animation::modulation(Animation_type type, float mod) const noexcept {
		// TODO[SEB]: FIXME: uncheckt out-of-range access, move to sprite_comp (_data should be immutable)
		// _data->animations.find(type)->second.modulation = mod;
	}

	bool Animation::animation_exists(Animation_type type) const noexcept {
		if(_data->animations.find(type) == _data->animations.end()){
			return false;
		}
		return true;
	}

	Time Animation::remaining_time(const Animation_type type, const float cur_frame) const noexcept {

		auto anim = _data->animations.find(type);
		if(anim == _data->animations.end())
			return 0_s;

		Animation_frame_data d = anim->second;
		// calculating remaining time in seconds for current animation
		if(d.frames<=1)
			return 0_s;

		float r_time = (d.frames - cur_frame) / (d.modulation * d.fps);

		return (second * r_time);
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
	glm::vec4 Animation::uv(const int frame, const Animation_type type) const noexcept{

		// Calculating corresponding uv-coords
		// uv-coords -> 1: x = xStart from left | 2: y = yStart from down | 3: z = xEnd from left | 4: w = yEnd from down

		// TODO: [foe] check const violation in
		_data->currentAnim = type;
		int row = _data->animations.find(type) -> second.row;

		float width = _data->frame_width / static_cast<float>(_data->texture->width());
		float height = _data->frame_height / static_cast<float>(_data->texture->height());
		float startX = frame * width;
		float startY = 1 - height - (row * height);
		const glm::vec4 uv = glm::vec4(startX, startY, startX + width, startY + height);

		return uv;
	}

	float Animation::next_frame(const Animation_type type, const float cur_frame, const float deltaTime, const bool repeat) const noexcept{
		int max_frames = _data->animations.find(type)->second.frames;
		int fps = _data->animations.find(type)->second.fps * _data->animations.find(type)->second.modulation;
		float ret = cur_frame;

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
		sf2::deserialize_json(in, [&](auto& msg, uint32_t row, uint32_t column) {
			ERROR("Error parsing JSON from "<<in.aid().str()<<" at "<<row<<":"<<column<<": "<<msg);
		}, *r);

		r->texture = in.manager().load<renderer::Texture>(r->texName);

		// Generating new Animation-Shared-Ptr and set _data-ptr to what r pointed to
		auto anim = std::make_shared<renderer::Animation>(std::move(r));

		return anim;
	}

	void Loader<renderer::Animation>::store(ostream out, const renderer::Animation& asset) throw(Loading_failed) {
		asset._data->texName = asset._data->texture.aid().name();
		sf2::serialize_json(out, asset);
	}
}
}
