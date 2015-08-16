#include "input_controller.hpp"


namespace mo {
namespace sys {
namespace controller {

	using namespace util;
	using namespace std::placeholders;


	Keyboard_controller::Keyboard_controller(
			Mapping_ptr mapping, util::signal_source<Quit_event>& quit_events,
	        std::function<glm::vec2(glm::vec2)>& screen_to_world_coords,
	        signal_source<SDL_KeyboardEvent>& keys,
	        signal_source<SDL_MouseMotionEvent>& mouse,
	        signal_source<SDL_MouseButtonEvent>& button)
		: Input_controller_base(mapping, quit_events),
	      _screen_to_world_coords(screen_to_world_coords),
		  _key_events(&Keyboard_controller::_on_key, this),
		  _mouse_events(&Keyboard_controller::_on_mouse_moved, this),
		  _button_events(&Keyboard_controller::_on_button_clicked, this),
		  _mouse_pos(0,0) {
		_key_events.connect(keys);
		_mouse_events.connect(mouse);
		_button_events.connect(button);
	}
	void Keyboard_controller::operator()(Controllable_interface& c) {
		auto move = glm::vec2 {0,0};

		if(_move_up>0)    move.y--;
		if(_move_down>0)  move.y++;
		if(_move_left>0)  move.x--;
		if(_move_right>0) move.x++;

		if(move.x!=0 || move.y!=0) {
			c.move(move);
		}

		if(_attack)
			c.attack();

		if(_use) {
			c.use();
			c.take();
		}

		for(uint8_t w=0; w<weapon_count; ++w)
			if(_weapon[w]==1)
				c.switch_weapon(w);

		if(_mapping->mouse_look && _mouse_look)
			c.look_at(_screen_to_world_coords(_mouse_pos));
	}

	void Keyboard_controller::_on_key(SDL_KeyboardEvent event) {
		auto iter = _mapping->key_action.find(from_sdl_keycode(event.keysym.sym));
		if(iter==_mapping->key_action.end())
			return;

		_on_command(iter->second, event.state==SDL_PRESSED);

	}

	void Input_controller_base::on_frame() {
		for(uint8_t w=0; w<weapon_count; ++w)
			if(_weapon[w]==1)
				_weapon[w]=2;
	}

	void Input_controller_base::_on_command(Command cmd, bool active) {
		switch(cmd) {
			case Command::move_up:
				_move_up+= active ? 1 : -1;
				break;

			case Command::move_down:
				_move_down+= active ? 1 : -1;
				break;

			case Command::move_left:
				_move_left+= active ? 1 : -1;
				break;

			case Command::move_right:
				_move_right+= active ? 1 : -1;
				break;

			case Command::attack:
				_attack = active;
				break;

			case Command::use_or_take:
				_use = active;
				break;

			case Command::weapon_0:
				if(!active)
					_weapon[0] = 0;
				else if(_weapon[0]==0)
					_weapon[0] = 1;
				break;

			case Command::weapon_1:
				if(!active)
					_weapon[1] = 0;
				else if(_weapon[1]==0)
					_weapon[1] = 1;
				break;

			case Command::weapon_2:
				if(!active)
					_weapon[2] = 0;
				else if(_weapon[2]==0)
					_weapon[2] = 1;
				break;

			case Command::weapon_3:
				if(!active)
					_weapon[3] = 0;
				else if(_weapon[3]==0)
					_weapon[3] = 1;
				break;

			case Command::enter_leave_game:
				break;
			case Command::quit:
				if(active)
					_quit_pressed = true;

				else if(_quit_pressed) {
					_quit_pressed = false;
					quit_events.inform(Quit_event{});
				}
				break;
		}
	}
	void Keyboard_controller::_on_mouse_moved(SDL_MouseMotionEvent event) {
		if(std::abs(_mouse_pos.x-event.x)>1 || std::abs(_mouse_pos.y-event.y)>1)
			_mouse_look = true;

		_mouse_pos.x = event.x;
		_mouse_pos.y = event.y;
	}

	void Keyboard_controller::_on_button_clicked(SDL_MouseButtonEvent event) {
		auto iter = _mapping->mouse_button_action.find(event.button);
		if(iter==_mapping->mouse_button_action.end())
			return;

		_on_command(iter->second, event.state==SDL_PRESSED);
	}

	Gamepad_controller::Gamepad_controller(Mapping_ptr mapping, util::signal_source<Quit_event>& quit_events, SDL_GameController* controller,
					  signal_source<Controller_added_event>& added_events,
					  signal_source<Controller_removed_event>& removed_events)
		: Input_controller_base(mapping, quit_events), _controller(controller),
		  _added_events(added_events), _removed_events(removed_events), _active(false), _pressed{0} {

		auto name = SDL_GameControllerName(controller);

		_haptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(controller));
		if(!_haptic)
			WARN("Warning: Controller '"<<name<<
					   "'' does not support haptics: "<< SDL_GetError());

		else {
			if( SDL_HapticRumbleInit(_haptic) < 0 ) {
				_haptic = nullptr;
				WARN("Warning: Unable to initialize rumble for '"<<name<<
						   "': "<<SDL_GetError());
			}
		}

		INFO("Detected gamepad '"<<name<<"'");
	}
	Gamepad_controller::~Gamepad_controller() {
		if(_haptic)
			SDL_HapticClose(_haptic);

		SDL_GameControllerClose(_controller);
	}


	void Gamepad_controller::on_frame() {
		Input_controller_base::on_frame();

		auto stick = [&](auto s) {
			glm::vec2 v {
				SDL_GameControllerGetAxis(_controller, to_sdl_axis_x(s)) / _mapping->stick_max_value,
				SDL_GameControllerGetAxis(_controller, to_sdl_axis_y(s)) / _mapping->stick_max_value,
			};

			auto dz = _mapping->stick_dead_zone;
			auto length = glm::length(v);

			if(length<dz)
				return glm::vec2{0,0};

			return v/length * ((length-dz)/(1-dz));
		};

		auto trigger = [&](auto t) {
			return glm::abs(SDL_GameControllerGetAxis(_controller,t))>_mapping->stick_max_value*_mapping->stick_dead_zone;
		};


		_move.x=_move.y=0;
		for(auto s : _mapping->move_sticks) {
			_move+=stick(s);
		}

		_look.x=_look.y=0;
		for(auto s : _mapping->aim_sticks) {
			_look+=stick(s);
		}


		bool pressed_now[command_count] {false};

		for(auto& button : _mapping->pad_button_action) {
			bool down = false;

			if(!is_trigger(button.first)) {
				down = SDL_GameControllerGetButton(_controller, to_sdl_pad_button(button.first))==1;

			} else if(button.first==Pad_button::left_trigger) {
				down = trigger(SDL_CONTROLLER_AXIS_TRIGGERLEFT);

			} else if(button.first==Pad_button::right_trigger)
				down = trigger(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
			else
				DEBUG("Unhandled controller button: "<<static_cast<int>(button.first));

			if(down)
				pressed_now[static_cast<std::size_t>(button.second)] = true;
		}

		std::size_t i=0;
		for(bool& p : _pressed) {
			if(p!=pressed_now[i]) {
				Command c = static_cast<Command>(i);

				_on_command(c, pressed_now[i]);
				if(pressed_now[i] && c==Command::enter_leave_game) {
					enter_or_leave();
				}

				p = pressed_now[i];
			}
			i++;
		}
	}

	void Gamepad_controller::operator()(Controllable_interface& c) {
		if(_move.x!=0 || _move.y!=0) {
			c.move(_move);
		}

		if(_look.x!=0 || _look.y!=0)
			c.look_in_dir(_look);

		if(_attack)
			c.attack();

		if(_use) {
			c.use();
			c.take();
		}

		for(auto w : range(weapon_count))
			if(_weapon[w]==1)
				c.switch_weapon(w);
	}
	void Gamepad_controller::feedback(float force) {
		DEBUG("gamepad-feedback: "<<force);
		if(SDL_HapticRumblePlay(_haptic, glm::clamp(force,0.f,1.f), 500)!=0)
			DEBUG("Rumble failed: "<<SDL_GetError()); // clear error
	}

	void Gamepad_controller::request_unjoin() {
		if(_active) {
			_removed_events.inform(Controller_removed_event{*this});
			_active = false;
		}
	}

	void Gamepad_controller::request_join() {
		if(!_active) {
			_added_events.inform(Controller_added_event{*this});
			_active = true;
		}
	}

	void Gamepad_controller::enter_or_leave() {
		if(_active) {
			_removed_events.inform(Controller_removed_event{*this});
			_active = false;
		} else {
			_added_events.inform(Controller_added_event{*this});
			_active = true;
		}
	}

	SDL_JoystickID Gamepad_controller::instance_id()const {
		return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(_controller));
	}

	Combined_controller::Combined_controller(
	        Keyboard_controller& keyboard,
	        std::vector<std::unique_ptr<Gamepad_controller>>& gamepads)
		: _keyboard(keyboard), _gamepads(gamepads) {
	}

	void Combined_controller::operator()(Controllable_interface& c) {
		_keyboard(c);

		for(auto& gp : _gamepads) {
			if(gp->is_used())
				_keyboard.tmp_disable_mouse_look();

			(*gp)(c);
		}
	}
	void Combined_controller::feedback(float force) {
		for(auto& gp : _gamepads)
			if(_keyboard.mouse_look_disabled())
				gp->feedback(force);

		_keyboard.feedback(force);
	}

}
}
}
