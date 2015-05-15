#include "texture.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <soil/SOIL.h>

namespace mo {
namespace renderer {

Texture::Texture(const std::string& path) throw(Texture_loading_failed) {
	_handle = SOIL_load_OGL_texture
	(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA,
		&_width,
		&_height
	);

	if(!_handle)
		throw Texture_loading_failed(SOIL_last_result());

	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(std::vector<uint8_t> buffer) throw(Texture_loading_failed) {
	_handle = SOIL_load_OGL_texture_from_memory
	(
		buffer.data(),
		buffer.size(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA,
		&_width,
		&_height
	);

	if(!_handle)
		throw Texture_loading_failed(SOIL_last_result());

	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int width, int height, std::vector<uint8_t> rgbaData) : _width(width), _height(height) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures( 1, &_handle );
	glBindTexture( GL_TEXTURE_2D, _handle );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData.data());
}
Texture::~Texture()noexcept {
	if(_handle!=0)
		glDeleteTextures(1, &_handle);
}

Texture& Texture::operator=(Texture&& s) {
	if(_handle!=0)
		glDeleteTextures(1, &_handle);

	_handle = s._handle;
	s._handle = 0;

	_width = s._width;
	_height = s._height;

	return *this;
}


void Texture::bind()const {
	glBindTexture(GL_TEXTURE_2D, _handle);
}
void Texture::unbind()const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

} /* namespace renderer */
}
