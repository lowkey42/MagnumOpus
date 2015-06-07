#include "engine.hpp"

#include "utils/log.hpp"

#include "configuration.hpp"
#include "input_manager.hpp"
#include "renderer/graphics_ctx.hpp"
#include "audio/sound_ctx.hpp"
#include "asset/asset_manager.hpp"

#include "audio/sound.hpp"

#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


namespace mo {
	namespace {
		void init_sub_system(Uint32 f, const std::string& name, bool required=true) {
			if(SDL_InitSubSystem(f)!=0) {
				auto m = "Could not initialize "+name+": "+get_sdl_error();

				if(required)
					FAIL(m);
				else
					WARN(m);
			}
		}
	}

auto get_sdl_error() -> std::string {
	std::string sdl_error(SDL_GetError());
	SDL_ClearError();
	return sdl_error;
}

Sdl_wrapper::Sdl_wrapper() {
	INVARIANT(SDL_Init(0)==0, "Could not initialize SDL: "<<get_sdl_error());

	init_sub_system(SDL_INIT_AUDIO, "SDL_Audio");
	init_sub_system(SDL_INIT_VIDEO, "SDL_Video");
	init_sub_system(SDL_INIT_JOYSTICK, "SDL_Joystick");
	init_sub_system(SDL_INIT_HAPTIC, "SDL_Haptic", false);
	init_sub_system(SDL_INIT_GAMECONTROLLER, "SDL_Gamecontroller");
	init_sub_system(SDL_INIT_EVENTS, "SDL_Events");
}
Sdl_wrapper::~Sdl_wrapper() {
	SDL_Quit();
}

Engine::Engine(const std::string& title, Configuration cfg)
  : _asset_manager(std::make_unique<asset::Asset_manager>(cfg.get("exe").get_or_throw(), title)),
    _configuration(std::make_unique<Configuration>(std::move(cfg))),
    _sdl(),
	_graphics_ctx(std::make_unique<renderer::Graphics_ctx>(title, *_asset_manager)),
	_sound_ctx(std::make_unique<audio::Sound_ctx>("SoundContext", *_asset_manager)),
	_input_manager(std::make_unique<Input_manager>()), _current_time(SDL_GetTicks() / 1000.0f) {
}

Engine::~Engine() noexcept = default;

auto Engine::enter_screen(std::unique_ptr<Screen> screen) -> Screen& {
	if(!_screen_stack.empty())
		_screen_stack.back()->_on_leave(*screen);

	screen->_on_enter(util::justPtr(_screen_stack.empty() ? nullptr : _screen_stack.back().get()));

	if(screen->_prev_screen_policy()==Prev_screen_policy::discard)
		_screen_stack.clear();

	_screen_stack.push_back(std::move(screen));

	return *_screen_stack.back().get();
}
void Engine::leave_screen(uint8_t depth) {
	if(depth<=0) return;

	auto last = std::shared_ptr<Screen>{};
	if(!_screen_stack.empty())
		last = std::move(_screen_stack.back());

	for(;depth>0; depth--)
		_screen_stack.pop_back();

	if(_screen_stack.empty()) {
		_quit=true;
		last->_on_leave(util::nothing());

	} else {
		last->_on_leave(util::justPtr(_screen_stack.back().get()));
		_screen_stack.back()->_on_enter(util::justPtr(last.get()));
	}
}

void Engine::on_frame() {
	_last_time = _current_time;
	_current_time = SDL_GetTicks() / 1000.0f;
	const float delta_time = _current_time - _last_time;


	_graphics_ctx->start_frame();

	_input_manager->update(delta_time);

	_poll_events();

	_on_frame(delta_time);

	auto screen_stack = _screen_stack;

	const int screen_stack_size = screen_stack.size();
	int screen_index=screen_stack_size-1; //< has to be signed

	// update all screens until we reached one with PrevScreenPolicy < Update
	for(; screen_index>=0; screen_index--) {
		auto& s = screen_stack.at(screen_index);

		s->_update(delta_time);

		if(s->_prev_screen_policy()!=Prev_screen_policy::update)
			break;
	}

	// find last screen to draw (PrevScreenPolicy >= Draw)
	for(; screen_index>=0; screen_index--)
		if(screen_stack.at(screen_index)->_prev_screen_policy()!=Prev_screen_policy::draw)
			break;

	screen_index=std::max(screen_index, 0);

	// draw all screens in reverse order
	for(; screen_index<screen_stack_size; screen_index++)
		screen_stack.at(screen_index)->_draw(delta_time);

	_graphics_ctx->end_frame(delta_time);
}

void Engine::_poll_events() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			_quit = true;
		else
			_input_manager->handle_event(event);
	}
}

} /* namespace core */
