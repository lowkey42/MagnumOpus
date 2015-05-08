#include "animation.hpp"

#include <sf2/sf2.hpp>

namespace mo{
namespace renderer{

	struct Animation_data{
		int frame_width;
		int frame_height;
		Texture_ptr texture;
		std::string texName = texture.aid().name();
		Animation_type currentAnim = Animation_type::idle;
		std::unordered_map<Animation_type, Animation_frame_data> animations;
	};

	sf2_enumDef(Animation_type,
		sf2_value(idle),
		sf2_value(moving),
		sf2_value(attack)
	)

	sf2_structDef(Animation_frame_data,
		sf2_member(row),
		sf2_member(fps),
		sf2_member(frames)
	)

	sf2_structDef(Animation,
		sf2_member(frame_width),
		sf2_member(frame_height),
		sf2_member(texName),
		sf2_member(animations)
	)

}

namespace asset {

	std::shared_ptr<renderer::Animation> Loader<renderer::Animation>::load(istream in) throw(Loading_failed){
		auto r = std::make_shared<renderer::Animation>();

		std::string data = in.content();
		sf2::io::StringCharSource source(data);

		sf2::ParserDefChooser<renderer::Animation>::get().parse(source, *r.get());

		r->texture = in.manager().load<renderer::Texture>(r->texName);

		return r;
	}

	void Loader<renderer::Animation>::store(ostream out, renderer::Animation& asset) throw(Loading_failed) {
		asset.texName = asset.texture.aid().name();
		std::string data = sf2::writeString(asset);
		out.write(data.c_str(), data.length());
	}
}
}
