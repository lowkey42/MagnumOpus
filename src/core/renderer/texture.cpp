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
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(int width, int height) : _width(width), _height(height) {
		glGenTextures( 1, &_handle );
		glBindTexture( GL_TEXTURE_2D, _handle );
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	}

	Texture::Texture(int width, int height, std::vector<uint8_t> rgbaData) : _width(width), _height(height) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures( 1, &_handle );
		glBindTexture( GL_TEXTURE_2D, _handle );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData.data());
	}
	Texture::~Texture()noexcept {
		if(_handle!=0)
			glDeleteTextures(1, &_handle);
	}

	Texture::Texture(Texture&& s)noexcept
		: _handle(s._handle), _width(s._width), _height(s._width) {
		s._handle = 0;
	}
	Texture& Texture::operator=(Texture&& s)noexcept {
		if(_handle!=0)
			glDeleteTextures(1, &_handle);

		_handle = s._handle;
		s._handle = 0;

		_width = s._width;
		_height = s._height;

		return *this;
	}


	void Texture::bind(int index)const {
		auto tex = GL_TEXTURE0+index;
		INVARIANT(tex<GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "to many textures");
		glActiveTexture(tex);
		glBindTexture(GL_TEXTURE_2D, _handle);
	}
	void Texture::unbind(int index)const {
		auto tex = GL_TEXTURE0+index;
		INVARIANT(tex<GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "to many textures");
		glActiveTexture(tex);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Framebuffer::Framebuffer(int width, int height, bool depth_buffer)
		: Texture(width, height), _fb_handle(0), _db_handle(0) {

		glGenFramebuffers(1, &_fb_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, _fb_handle);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _handle, 0);

		if(depth_buffer) {
			glGenRenderbuffers(1, &_db_handle);
			glBindRenderbuffer(GL_RENDERBUFFER, _db_handle);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
									  GL_RENDERBUFFER, _db_handle);
		}

		INVARIANT(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE, "Couldn't create framebuffer!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	Framebuffer::Framebuffer(Framebuffer&& rhs)noexcept
		: Texture(std::move(rhs)), _fb_handle(rhs._fb_handle), _db_handle(rhs._db_handle) {

		rhs._fb_handle = 0;
		rhs._db_handle = 0;
	}
	Framebuffer::~Framebuffer()noexcept {
		if(_fb_handle)
			glDeleteFramebuffers(1, &_fb_handle);

		if(_db_handle)
			glDeleteRenderbuffers(1, &_db_handle);
	}
	Framebuffer& Framebuffer::operator=(Framebuffer&& rhs)noexcept {
		if(_fb_handle)
			glDeleteFramebuffers(1, &_fb_handle);

		if(_db_handle)
			glDeleteRenderbuffers(1, &_db_handle);

		Texture::operator=(std::move(rhs));

		_fb_handle = rhs._fb_handle;
		_db_handle = rhs._db_handle;

		rhs._fb_handle = 0;
		rhs._db_handle = 0;

		return *this;
	}

	void Framebuffer::set_viewport() {
		glViewport(0,0, width(), height());
	}

	void Framebuffer::clear(glm::vec3 color) {
		glClearColor(color.r, color.g, color.b, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | (_db_handle ? GL_DEPTH_BUFFER_BIT : 0));
	}

	void Framebuffer::bind_target() {
		glBindFramebuffer(GL_FRAMEBUFFER, _fb_handle);
	}
	void Framebuffer::unbind_target() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} /* namespace renderer */
}
