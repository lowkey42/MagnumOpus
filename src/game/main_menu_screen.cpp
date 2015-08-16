#include "main_menu_screen.hpp"

#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_screen.hpp"

#include <core/gui/button.hpp>

namespace mo {
	using namespace unit_literals;
	using namespace renderer;
	using namespace glm;

	namespace {
		constexpr Time fade = 2_s;
	}

	Main_menu_screen::Main_menu_screen(Game_engine& game_engine, bool ingame)
	    : Screen(game_engine),
	      Ui_container(game_engine, calculate_vscreen(game_engine, 512),
	                    - vec2{
	                       300,
	                       (100*3+5*4)
	                   }/2.f,
	                   vec2{150,0},
	                   gui::vertical(5)),
	      _ingame(ingame),
	      _screen_size(calculate_vscreen(game_engine, 512)),
	      _on_quit_slot(&Main_menu_screen::_on_quit, this),
	      _background(game_engine.assets().load<Texture>("tex:ui_intro"_aid)),
	      _circle(game_engine.assets().load<Texture>("tex:ui_intro_circle"_aid))
	{

		if(Game_screen::save_exists(game_engine)) {
			_root.add_new<gui::Button>("Continue", [ingame, &game_engine] {
				if(ingame) {
					DEBUG("=> game (continue)");
					game_engine.leave_screen();

				} else {
					try {
						game_engine.enter_screen<Game_screen>();

					} catch(util::Error e) { // load failed:
						// TODO: request player name
						game_engine.enter_screen<Game_screen>("TODO");
					}
				}
			});
		}

		_root.add_new<gui::Button>("New Game", [&game_engine]{
			// TODO: request player name
			game_engine.enter_screen<Game_screen>("TODO");
		});

		_root.add_new<gui::Button>("Quit", [&game_engine] {
			game_engine.exit();
		});
	}

	void Main_menu_screen::_on_enter(util::maybe<Screen&> prev) {
		enable();
		_on_quit_slot.connect(_game_engine.controllers().quit_events);
	}
	void Main_menu_screen::_on_leave(util::maybe<Screen&> next) {
		disable();
		_on_quit_slot.disconnect(_game_engine.controllers().quit_events);
	}

	void Main_menu_screen::_update(float delta_time) {
		if(_quit) {
			DEBUG("=> game");
			_quit = false;
			_game_engine.leave_screen();
			return;
		}

	//	_fade_left-=delta_time * second;
	//	_fadein_left-=delta_time * second;

		_time_acc+=delta_time * second;

		update_ui();
	}


	void Main_menu_screen::_draw(float time ) {
		renderer::Disable_depthtest ddt{};
		(void)ddt;

			_sprite_renderer.draw(
		            _background,
		            glm::vec2(0.f, 0.f),
		            glm::vec4{0.18f,0.18f,0.18f,0.18f},
		            glm::vec4(0,0,1,1),
		            glm::vec2(_camera.viewport().w*2.f, _camera.viewport().w));

			_sprite_renderer.draw(
		            _circle,
		            glm::vec2(0.f, 0.f),
		            glm::vec4{0.15f,0.15f,0.15f,1.f},
		            glm::vec4(0,0,1,1),
		            glm::vec2(_camera.viewport().w*2 *(800/2048.f), _camera.viewport().w*2 *(800/2048.f)),
		            glm::rotate(glm::mat4(), _time_acc/25_s, {0.f,0.f,1.f}));

		// TODO: draw highscore
		// TODO: draw credits

		draw_ui();
	}
}
