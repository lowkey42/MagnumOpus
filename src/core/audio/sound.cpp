#include "sound.hpp"

#ifndef EMSCRIPTEN
#	include <SDL2/SDL_mixer.h>
#else
#	include <SDL/SDL_mixer.h>
#endif

namespace mo {
namespace audio {

	Sound::Sound(asset::istream stream) throw(Sound_loading_failed) : _handle(nullptr, Mix_FreeChunk){


#ifndef EMSCRIPTEN
		auto buffer = stream.bytes();
		SDL_RWops* rw = SDL_RWFromMem(buffer.data(), buffer.size());
		if(!rw){
			WARN("SDL_RWFromMem ("<<stream.aid().str()<<") failed: " << SDL_GetError());
			return;
		}
		_handle.reset(Mix_LoadWAV_RW(rw, 1));

#else
		auto location = stream.physical_location();
		stream.close();

		if(location.is_nothing()) {
			WARN("Couldn't locate sound file: " << stream.aid().str());
			return;
		}

		_handle.reset(Mix_LoadWAV(location.get_or_throw().c_str()));
#endif

		if(!_handle){
			WARN("Mix_LoadWAV ("<<stream.aid().str()<<") failed: " << Mix_GetError());
		}

	}

} /* namespace sound */
}
