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
	constexpr auto default_cfg = Sounds_cfg{44100, 2, 4096};
#else
	constexpr auto default_cfg = Sounds_cfg{44100, 2, 2048};
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

	namespace audio {

		Sound_ctx::Sound_ctx(const std::string& name, asset::Asset_manager& assets){
			auto& cfg = asset::unpack(assets.load_maybe<Sounds_cfg>("cfg:sounds"_aid)).get_or_other(
				 default_cfg
			);

			DEBUG("frequence: " << cfg.frequence << " | channels: " << cfg.max_channels << " | buffer: " << cfg.buffer_size);

			if(Mix_OpenAudio(cfg.frequence, MIX_DEFAULT_FORMAT, cfg.max_channels, cfg.buffer_size) == 0) {
				DEBUG("Sound_ctx succesfully initialized!");
			} else {
				FAIL("Initializing Sound incomplete: " << Mix_GetError());
			}

			if(&cfg==&default_cfg) {
				assets.save<Sounds_cfg>("cfg:sounds"_aid, cfg);
			}
		}


		void Sound_ctx::play(std::shared_ptr<const audio::Sound> s, Angle angle, Distance dist, int loop) const noexcept {
			Mix_PlayChannel(-1, s->getSound(), loop);
		}


		void Sound_ctx::play(std::shared_ptr<const audio::Music> m, Time fade_time) const noexcept {
			Mix_PlayMusic(m->getMusic(), -1);
		}


		void Sound_ctx::sound_volume(std::shared_ptr<const audio::Sound> sound, int v) const noexcept {
			Mix_VolumeChunk(sound->getSound(), v);
		}


		void Sound_ctx::music_volume(int v) const noexcept {
			Mix_VolumeMusic(v);
		}


	}
}

