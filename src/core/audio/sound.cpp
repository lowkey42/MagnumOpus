#include "sound.hpp"

#ifndef __EMSCRIPTEN__
#	include <SDL2/SDL_mixer.h>
#else
#	include <SDL/SDL_mixer.h>
#endif

namespace mo {
namespace audio {

	Sound::Sound(asset::istream stream) : _handle(nullptr, Mix_FreeChunk){
		auto buffer = stream.bytes();
		SDL_RWops* rw = SDL_RWFromMem(buffer.data(), buffer.size());
		if(!rw){
			WARN("SDL_RWFromMem ("<<stream.aid().str()<<") failed: " << SDL_GetError());
			return;
		}
		_handle.reset(Mix_LoadWAV_RW(rw, 1));

		if(!_handle){
			WARN("Mix_LoadWAV ("<<stream.aid().str()<<") failed: " << Mix_GetError());
		}

	}

} /* namespace sound */
}
