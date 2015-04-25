#include "input_manager.hpp"

#include "asset/asset_manager.hpp"

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

namespace core {
	using namespace std::placeholders;


	Input_manager::Input_manager() {
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_GameControllerEventState(SDL_ENABLE);
	}

	Input_manager::~Input_manager()noexcept {
	}

	void Input_manager::update(float) {
	}


	void Input_manager::handle_event(SDL_Event& event) {
		switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if(event.key.repeat==0)
					keyboard_events.inform(event.key);
				break;

			case SDL_MOUSEMOTION:
				mouse_events.inform(event.motion);
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				button_events.inform(event.button);
				break;

			case SDL_CONTROLLERDEVICEADDED:
				DEBUG("Controller added: "<<event.cdevice.which);
				_add_gamepad(event.cdevice.which);
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				DEBUG("Controller removed: "<<event.cdevice.which);
				_remove_gamepad(event.cdevice.which);
				break;

			case SDL_CONTROLLERDEVICEREMAPPED:
				break; // ignored for now
		}
	}
	void Input_manager::_add_gamepad(int joystick_id) {
		if(joystick_id==-1)
			joystick_id = SDL_NumJoysticks()-1;

		if (SDL_IsGameController(joystick_id)) {
			SDL_GameController* controller = SDL_GameControllerOpen(joystick_id);
			if (controller) {
				_gamepads.push_back(controller);
				gamepad_added_events.inform(controller);

			} else {
				std::cerr<<"Could not open gamecontroller "<<joystick_id<<": "<<SDL_GetError()<<std::endl;
			}
		}
	}

	void Input_manager::_remove_gamepad(int instance_id) {
		auto e = std::find_if(_gamepads.begin(), _gamepads.end(),
					   [instance_id](SDL_GameController* c) {
			return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c))==instance_id;
		});

		if(e!=_gamepads.end()) {
			gamepad_removed_events.inform(*e);

			std::swap(*e, _gamepads.back());

			_gamepads.pop_back();
		}
	}


}
