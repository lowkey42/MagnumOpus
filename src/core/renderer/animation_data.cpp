#include "animation_data.hpp"

#include <sf2/sf2.hpp>

namespace mo{
namespace renderer{

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

	sf2_structDef(Animation_data,
		sf2_member(frame_width),
		sf2_member(frame_height),
		sf2_member(texName),
		sf2_member(animations)
	)

}

namespace asset {

	std::shared_ptr<renderer::Animation_data> Loader<renderer::Animation_data>::load(istream in) throw(Loading_failed){
		auto r = std::make_shared<renderer::Animation_data>();

		std::string data = in.content();
		sf2::io::StringCharSource source(data);

		sf2::ParserDefChooser<renderer::Animation_data>::get().parse(source, *r.get());

		return r;
	}

	void Loader<renderer::Animation_data>::store(ostream out, renderer::Animation_data& asset) throw(Loading_failed) {
		std::string data = sf2::writeString(asset);
		out.write(data.c_str(), data.length());
	}
}
}
