#include "graphics_ctx.hpp"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <GL/glew.h>
#include "../utils/log.hpp"

namespace {
	void sdl_error_check() {
		const char *err = SDL_GetError();
		if(*err != '\0') {
			std::string errorStr(err);
			SDL_ClearError();
			FAIL("SDL: "<<errorStr);
		}
	}

#ifndef EMSCRIPTEN
	void
#ifdef GLAPIENTRY
	GLAPIENTRY
#endif
	gl_debug_callback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void*) {
		WARN(std::string(message,length)<<" (source: "<<source<<", type: "<<type<<", id: "<<id<<", severity: "<<severity<<")");
	}
#endif
}

namespace mo {
namespace renderer {
	Graphics_ctx::Graphics_ctx(const std::string& name, int width, int height, bool fullscreen)
	 : _name(name), _win_width(width), _win_height(height), _window(nullptr, SDL_DestroyWindow) {

#ifndef EMSCRIPTEN
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		int win_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if(fullscreen)
			win_flags|=SDL_WINDOW_FULLSCREEN_DESKTOP;

		_window.reset( SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							_win_width, _win_height, win_flags) );

		if (!_window)
			FAIL("Unable to create window");

		sdl_error_check();

		SDL_GetWindowSize(_window.get(), &_win_width, &_win_height);

		try {
			_gl_ctx = SDL_GL_CreateContext(_window.get());
			sdl_error_check();
			SDL_GL_MakeCurrent(_window.get(), _gl_ctx);
			sdl_error_check();

		} catch (const std::runtime_error& ex) {
			FAIL("Failure to create OpenGL context. This application requires a OpenGL 3.3 capable GPU. Error was: "<< ex.what());
		}

		if(SDL_GL_SetSwapInterval(-1)) SDL_GL_SetSwapInterval(1);

		glewExperimental = GL_TRUE;
		glewInit();

#ifndef EMSCRIPTEN
		INVARIANT(GLEW_VERSION_3_3, "Requested OpenGL 3.3 Context but 3.3 Features are not available.");

		if(GLEW_KHR_debug){
			glDebugMessageCallback((GLDEBUGPROC)gl_debug_callback, stderr);
		}
		else{
			WARN("No OpenGL debug log available.");
		}
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		set_clear_color(0.0f,0.0f,0.0f);
	}

	Graphics_ctx::~Graphics_ctx() {
		SDL_GL_DeleteContext(_gl_ctx);
	}

	void Graphics_ctx::start_frame() {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		_frame_start_time = SDL_GetTicks() / 1000.0f;
	}
	void Graphics_ctx::end_frame(float delta_time) {
		float smooth_factor=0.1f;
		_delta_time_smoothed=(1.0f-smooth_factor)*_delta_time_smoothed+smooth_factor*delta_time;

		float cpu_delta_time = SDL_GetTicks() / 1000.0f - _frame_start_time;
		_cpu_delta_time_smoothed=(1.0f-smooth_factor)*_cpu_delta_time_smoothed+smooth_factor*cpu_delta_time;

		_time_since_last_FPS_output+=delta_time;
		if(_time_since_last_FPS_output>=1.0f){
			_time_since_last_FPS_output=0.0f;
			std::ostringstream osstr;
			osstr<<_name<<" ("<<(int((1.0f/_delta_time_smoothed)*10.0f)/10.0f)<<" FPS, ";
			osstr<<(int(_delta_time_smoothed*10000.0f)/10.0f)<<" ms/frame, ";
			osstr<<(int(_cpu_delta_time_smoothed*10000.0f)/10.0f)<<" ms/frame [cpu])";
			SDL_SetWindowTitle(_window.get(), osstr.str().c_str());
		}
		SDL_GL_SwapWindow(_window.get());
	}
	void Graphics_ctx::set_clear_color(float r, float g, float b) {
		glClearColor(r,g,b,1.f);
	}


	Disable_depthtest::Disable_depthtest() {
		glDisable(GL_DEPTH_TEST);
	}

	Disable_depthtest::~Disable_depthtest() {
		glEnable(GL_DEPTH_TEST);
	}
}
}
