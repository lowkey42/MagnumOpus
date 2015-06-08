#include "sound_ctx.hpp"

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#include <SDL2/SDL_mixer.h>

namespace {

	struct Sounds_cfg {
		int frequence;
		int mono_stereo;
		int max_channels;
		int buffer_size;
	};

	sf2_structDef(Sounds_cfg,
		sf2_member(frequence),
		sf2_member(mono_stereo),
		sf2_member(max_channels),
		sf2_member(buffer_size)
	)

#ifndef EMSCRIPTEN
	constexpr auto default_cfg = Sounds_cfg{44100, 2, 1024, 4096};
#else
	constexpr auto default_cfg = Sounds_cfg{44100, 2, 256, 2048};
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

		Sound_ctx::Sound_ctx(const std::string& name, asset::Asset_manager& assets)
			: _max_channels(asset::unpack(assets.load_maybe<Sounds_cfg>("cfg:sounds"_aid)).get_or_other(default_cfg).max_channels) {
			auto& cfg = asset::unpack(assets.load_maybe<Sounds_cfg>("cfg:sounds"_aid)).get_or_other(
				 default_cfg
			);

			// Checking if frequence is faulty -> setting to 44100MHz
			int verified_frequence = (cfg.frequence % 22050 == 0) ? cfg.frequence : 44100;
			DEBUG("frequence: " << verified_frequence << " | " << ((cfg.mono_stereo == 1) ? "Mono" : "Stereo")
								<< " | max_channels: " << cfg.max_channels << " | buffer: " << cfg.buffer_size);

			// Open SDL Audio Mixer
			if(Mix_OpenAudio(verified_frequence, MIX_DEFAULT_FORMAT, cfg.mono_stereo, cfg.buffer_size) == 0) {
				DEBUG("Sound_ctx succesfully initialized!");
			} else {
				FAIL("Initializing Sound incomplete: " << Mix_GetError());
			}

			Mix_AllocateChannels(cfg.max_channels);

			if(&cfg==&default_cfg) {
				assets.save<Sounds_cfg>("cfg:sounds"_aid, cfg);
			}
		}


		Channel_id Sound_ctx::play(std::shared_ptr<const audio::Sound> s, Angle angle, Distance dist, int loop) noexcept {
			Channel_id curID = Mix_PlayChannel(_curMixPos, s->getSound(), loop);
			Mix_SetPosition(curID, angle.value(), dist.value());
			_curMixPos++;

			// Assuming that there is no need of 1024 sounds at once
			if(_curMixPos > _max_channels){
				_curMixPos = curID = 0;
			}

			return curID;
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


		void Sound_ctx::stop(Channel_id id) const noexcept {
			Mix_HaltChannel(id);
		}


		void Sound_ctx::update_position(Channel_id id, Angle angle, Distance dist) const noexcept {
			Mix_SetPosition(id, angle.value(), dist.value());
		}


	}
}

