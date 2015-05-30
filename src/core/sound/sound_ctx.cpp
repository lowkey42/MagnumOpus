#include "sound_ctx.hpp"

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#include <SDL2/SDL_mixer.h>

namespace {

	struct Sounds_cfg {
		int frequence;
		int max_channels;
		int buffer_size;
	};

	sf2_structDef(Sounds_cfg,
		sf2_member(frequence),
		sf2_member(max_channels),
		sf2_member(buffer_size)
	)

#ifndef EMSCRIPTEN
	constexpr auto default_cfg = Sounds_cfg{44100, 32, 4096};
#else
	constexpr auto default_cfg = Sounds_cfg{44100, 128, 2048};
#endif

}


namespace mo {
namespace asset {
	template<>
	struct Loader<Sounds_cfg> {
		using RT = std::shared_ptr<Sounds_cfg>;

		static RT load(istream in) throw(Loading_failed) {
			auto r = std::make_shared<Sounds_cfg>();

			sf2::parseStream(in, *r);

			return r;
		}

		static void store(ostream out, const Sounds_cfg& asset) throw (Loading_failed) {
			sf2::writeStream(out, asset);
		}
	};
}

	namespace sound {

		Sound_ctx::Sound_ctx(const std::string& name, asset::Asset_manager& assets){
			auto& cfg = asset::unpack(assets.load_maybe<Sounds_cfg>("cfg:sounds"_aid)).get_or_other(
				 default_cfg
			);

			Mix_OpenAudio(cfg.frequence, MIX_DEFAULT_FORMAT, cfg.max_channels, cfg.buffer_size);

			if(&cfg==&default_cfg) {
				assets.save<Sounds_cfg>("cfg:sounds"_aid, cfg);
			}

		}

	}
}

