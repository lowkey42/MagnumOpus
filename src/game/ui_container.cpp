#include "ui_container.hpp"

#include "sys/controller/controller.hpp"

#include <core/renderer/graphics_ctx.hpp>

namespace mo {

	using namespace unit_literals;
	using namespace renderer;

	struct Ui_container::ui_controller : sys::controller::Controllable_interface{

		ui_controller(gui::Widget& w) : w(w){}

		void after_update() {
			if(activate)
				w.on_activate();

			if(dir_set) {
				if((dir_set_counter%40) == 0) {
					w.on_move(dir);

				}
				dir_set_counter++;

			} else
				dir_set_counter = 0;

			activate = false;
			dir_set = false;
		}

		void move(glm::vec2 direction) override {
			if(direction.x>0)
				set_dir(gui::Direction::right);
			else if(direction.x<0)
				set_dir(gui::Direction::left);
			else if(direction.y<0)
				set_dir(gui::Direction::up);
			else if(direction.y>0)
				set_dir(gui::Direction::down);
		}
		void look_at(glm::vec2 position) override {
			if(last_mouse_pos!=position) {
				last_mouse_pos = position;
				w.on_move(position);
			}
		}
		void look_in_dir(glm::vec2 direction) override {
			if(direction.x>0)
				set_dir(gui::Direction::right);
			else if(direction.x<0)
				set_dir(gui::Direction::left);
			else if(direction.y<0)
				set_dir(gui::Direction::up);
			else if(direction.y>0)
				set_dir(gui::Direction::down);
		}
		void attack() override {
			activate = true;
		}
		void use() override {
			activate = true;
		}
		void take() override {
			activate = true;
		}
		void switch_weapon(uint32_t weapon_id) override {
			activate = true;
		}

		void set_dir(gui::Direction d) {
			dir = d;
			dir_set = true;
		}

		gui::Widget& w;
		glm::vec2 last_mouse_pos;
		bool activate = false;
		gui::Direction dir = gui::Direction::up;
		bool dir_set = false;
		int dir_set_counter = 0;
	};


	Ui_container::Ui_container(Game_engine& game_engine,
	                           glm::vec2 screen_size, glm::vec2 position,
	                           glm::vec2 size, gui::Layout layout)
	    : _game_engine(game_engine),
	      _camera(screen_size),
	      _mouse_camera(glm::vec2{game_engine.graphics_ctx().win_width(),
	                              game_engine.graphics_ctx().win_height()}),
	      _sprite_renderer(game_engine.assets()),
	      _text_renderer(game_engine.assets()),
	      _font(_game_engine.assets().load<Font>("font:menu_font"_aid)),
	      _root(*this, position, size, layout),
	      _updates_to_skip(30),
	      _controller(std::make_unique<ui_controller>(_root)),
	      _key_events(&Ui_container::_on_key, this) {
	}
	Ui_container::~Ui_container() = default;

	auto Ui_container::create_texture(const std::string& type) -> gui::Ui_texture {
		return _game_engine.assets().load<Texture>(asset::AID(asset::Asset_type::tex, "ui_"+type));
	}
	auto Ui_container::create_text(const std::string& str) -> gui::Ui_text {
		auto text = Text_dynamic(_font);
		text.set(str);

		return text;
	}

	void Ui_container::draw(gui::Ui_texture& tex, glm::vec2 center, bool highlight,
		              glm::vec4 clip) {

		_sprite_renderer.draw(tex, center, glm::vec4(1,1,1,1), clip);
	}

	void Ui_container::draw(gui::Ui_text& tex, glm::vec2 center, bool highlight) {
		_text_renderer.draw(tex,
		                    center,
		                    highlight ? glm::vec4(1.0,0.8,0.8,1) : glm::vec4(0.6,0.2,0.2,1), 0.5f );
	}

	void Ui_container::play_activate() {
		// TODO
	}
	void Ui_container::play_focus() {
		// TODO
	}

	void Ui_container::draw_ui() {
		_sprite_renderer.set_vp(_camera.vp());
		_text_renderer.set_vp(_camera.vp());

		_root.draw(true);
	}
	void Ui_container::update_ui() {
		if(_updates_to_skip<=0) {
			_game_engine.controllers().main_controller()(*_controller);
			_controller->after_update();
			// TODO: Text-input events

		} else
			_updates_to_skip--;
	}

	void Ui_container::_on_key(SDL_KeyboardEvent e) {
		if(e.repeat==0 && e.state==SDL_RELEASED) {
			auto key = e.keysym.sym;
			if(key>=33 && key<=126) {
				std::string str;
				if(key>=97 && key<=122 && (e.keysym.mod==KMOD_LSHIFT || e.keysym.mod==KMOD_RSHIFT))
					str.push_back(key-32);
				else
					str.push_back(key);

				_root.on_input(str);

			} else if(key==SDLK_RETURN || key==SDLK_ESCAPE) {
				_root.on_input("\r");

			} else if(key==SDLK_BACKSPACE) {
				_root.on_input("\b");
			}
		}
	}

	void Ui_container::enable() {
		_game_engine.controllers().screen_to_world_coords([&](glm::vec2 p){
			return this->_mouse_camera.screen_to_world(p);
		});
		_key_events.connect(_game_engine.input().keyboard_events);
	}

	void Ui_container::disable() {
		_game_engine.controllers().screen_to_world_coords([](glm::vec2 p){
			return p;
		});
		_key_events.disconnect(_game_engine.input().keyboard_events);
	}

}
