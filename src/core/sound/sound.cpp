#include "sound.hpp"

namespace mo {
namespace sound {

	Sound::Sound(std::vector<uint8_t> buffer) throw(Sound_loading_failed) : _data(nullptr, Mix_FreeChunk){

		SDL_RWops* rw = SDL_RWFromMem(buffer.data(), buffer.size());
		_data.reset(Mix_LoadWAV_RW(rw, 1));
		if(_data.get() == NULL){
			DEBUG("Mix_LoadWAV_RW returned NULL -> " << _data.get() << "  Problem: " << Mix_GetError());
		}

	}

	Sound& Sound::operator=(Sound&& s) noexcept {
		if(_data != nullptr)
			Mix_FreeChunk(_data.get());

		_data.reset(s._data.release());
		s._data.~unique_ptr();

		return *this;
	}

} /* namespace sound */
}
