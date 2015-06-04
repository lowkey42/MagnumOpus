#include "music.hpp"

namespace mo {
namespace audio {

	namespace {

		int64_t istream_seek( struct SDL_RWops *context, int64_t offset, int whence) {
			std::istream* stream = (std::istream*) context->hidden.unknown.data1;

			stream->clear();

			if ( whence == SEEK_SET )
				stream->seekg ( offset, std::ios::beg );
			else if ( whence == SEEK_CUR )
				stream->seekg ( offset, std::ios::cur );
			else if ( whence == SEEK_END )
				stream->seekg ( offset, std::ios::end );

			return stream->fail() ? -1 : static_cast<int64_t>(stream->tellg());
		}


		std::size_t istream_read(SDL_RWops *context, void *ptr, std::size_t size, std::size_t maxnum) {
			if ( size == 0 )
				return -1;

			std::istream* stream = (std::istream*) context->hidden.unknown.data1;
			stream->read( (char*)ptr, size * maxnum );

			return stream->bad() ? -1 : stream->gcount() / size;
		}

		int istream_close( SDL_RWops *context ) {
			if ( context ) {
				SDL_FreeRW( context );
			}
			return 0;
		}

	}

	Music::Music(asset::istream stream) throw(Music_loading_failed) :
	    _handle(nullptr, Mix_FreeMusic), _stream(std::make_unique<asset::istream>(std::move(stream))){

		SDL_RWops *rwops = SDL_AllocRW();
		INVARIANT(rwops, "SDL_AllocRW failed");

		rwops->seek = istream_seek;
		rwops->read = istream_read;
		rwops->write = NULL;
		rwops->close = istream_close;
		rwops->hidden.unknown.data1 = _stream.get();

		_handle.reset(Mix_LoadMUS_RW(rwops, 1));
		if(_handle.get() == NULL){
			DEBUG("Mix_LoadMUS_RW returned NULL -> " << _handle.get() << "  Problem: " << Mix_GetError());
		}

	}

} /* namespace sound */
}
