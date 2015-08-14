#include "menu_screen.hpp"

#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_screen.hpp"


namespace mo {
	using namespace unit_literals;
	using namespace renderer;

	namespace {
		struct ui_controller : sys::controller::Controllable_interface{

			ui_controller(Game_engine& engine) : _engine(engine){}

			void move(glm::vec2 direction) override {}
			void look_at(glm::vec2 position) override {}
			void look_in_dir(glm::vec2 direction) override {}
			void attack() override { enter_game(); }
			void use() override { enter_game(); }
			void take() override { enter_game(); }
			void switch_weapon(uint32_t weapon_id) override { enter_game(); }

			void enter_game(){
				// TODO: move to main-menu
				if(Game_screen::save_exists(_engine))
					//_engine.enter_screen<Game_screen>();
					;
				else {
					INFO("no savegame");
					//_engine.enter_screen<Game_screen>("TODO");
				}
			}

			Game_engine& _engine;

		};

		constexpr Time fade = 2_s;
	}

	Menu_Screen::Menu_Screen(Game_engine& game_engine) :
		Screen(game_engine),
		_game_engine(game_engine),
	    _camera(calculate_vscreen(game_engine, 512)),
		_newGame(game_engine.assets(),  game_engine.assets().load<Texture>("tex:ui_new1"_aid), 300, 100),
		_loadGame(game_engine.assets(), game_engine.assets().load<Texture>("tex:ui_load1"_aid), 300, 100),
		_settings(game_engine.assets(), game_engine.assets().load<Texture>("tex:ui_settings1"_aid), 300, 100),
		_quit(game_engine.assets(), game_engine.assets().load<Texture>("tex:ui_quit1"_aid), 300, 100),
		_fade_left(fade + 1_s)
	{
	}


	void Menu_Screen::_update(float delta_time) {
		_fade_left-=delta_time * second;

		ui_controller uic(_game_engine);
		_game_engine.controllers().main_controller()(uic);

	}


	void Menu_Screen::_draw(float time ) {
		renderer::Disable_depthtest ddt{};
		(void)ddt;

		//auto fp = glm::clamp(_fade_left/fade, 0.f, 1.f);

		_newGame.set_vp(_camera.vp());
		_newGame.set_color({1.f, 1.f, 1.f, 1.f});
		_newGame.draw(glm::vec2(0.0f, -180));

		_loadGame.set_vp(_camera.vp());
		_loadGame.set_color({1.f, 1.f, 1.f, 1.f});
		_loadGame.draw(glm::vec2(0.f, -60.f));

		_settings.set_vp(_camera.vp());
		_settings.set_color({1.f, 1.f, 1.f, 1.f});
		_settings.draw(glm::vec2(0.f, 60.f));

		_quit.set_vp(_camera.vp());
		_quit.set_color({1.f, 1.f, 1.f, 1.f});
		_quit.draw(glm::vec2(0.f, 180.f));
	}
}
