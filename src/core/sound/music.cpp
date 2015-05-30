#include "Music.hpp"

namespace mo {
namespace sound {

	Music::Music(std::vector<uint8_t> buffer) throw(Music_loading_failed) : _data(nullptr, Mix_FreeMusic){

		SDL_RWops* rw = SDL_RWFromMem(buffer.data(), buffer.size());
		_data.reset(Mix_LoadMUS_RW(rw, 1));
		if(_data.get() == NULL){
			DEBUG("Mix_LoadMUS_RW returned NULL -> " << _data.get() << "  Problem: " << Mix_GetError());
		}

	}

	Music& Music::operator=(Music&& s) noexcept {
		if(_data != nullptr)
			Mix_FreeMusic(_data.get());

		_data.reset(s._data.release());
		s._data.~unique_ptr();

		return *this;
	}

} /* namespace sound */
}
