#include "Music.hpp"

namespace mo {
namespace sound {

	Music::Music(std::vector<uint8_t> buffer) throw(Music_loading_failed) : _handle(nullptr, Mix_FreeMusic), _buffer(buffer){

		SDL_RWops* rw = SDL_RWFromMem(_buffer.data(), _buffer.size());
		//SDL_RWops* rw = SDL_RWFromMem(buffer.data(), buffer.size());
		_handle.reset(Mix_LoadMUS_RW(rw, 1));
		if(_handle.get() == NULL){
			DEBUG("Mix_LoadMUS_RW returned NULL -> " << _handle.get() << "  Problem: " << Mix_GetError());
		}

	}

	Music& Music::operator=(Music&& s) noexcept {
		if(_handle != nullptr)
			Mix_FreeMusic(_handle.get());

		_handle.reset(s._handle.release());
		s._handle.~unique_ptr();

		return *this;
	}

} /* namespace sound */
}
