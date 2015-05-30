#include "sound.hpp"

namespace mo {
namespace sound {

	Sound::Sound(std::vector<uint8_t> buffer) throw(Sound_loading_failed) : _handle(nullptr, Mix_FreeChunk){

		SDL_RWops* rw = SDL_RWFromMem(buffer.data(), buffer.size());
		_handle.reset(Mix_LoadWAV_RW(rw, 1));
		if(_handle.get() == NULL){
			DEBUG("Mix_LoadWAV_RW returned NULL -> " << _handle.get() << "  Problem: " << Mix_GetError());
		}

	}

	Sound& Sound::operator=(Sound&& s) noexcept {
		if(_handle != nullptr)
			Mix_FreeChunk(_handle.get());

		_handle.reset(s._handle.release());
		s._handle.~unique_ptr();

		return *this;
	}

} /* namespace sound */
}
