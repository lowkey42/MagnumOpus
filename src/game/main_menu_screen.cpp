#include "main_menu_screen.hpp"

#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_screen.hpp"

#include <core/gui/button.hpp>
#include <core/audio/music.hpp>

#include"highscore.hpp"

namespace mo {
	using namespace unit_literals;
	using namespace renderer;
	using namespace glm;

	namespace {
		auto load_highscore(asset::Asset_manager& assets) -> std::string {
			return print_scores(list_scores(assets));
		}

		auto load_credits(asset::Asset_manager& assets) -> std::string {
			std::stringstream ss;
			ss<<std::setw(25/2)<<"CREDITS"<<std::endl<<std::endl;
			ss<<"LEAD DEVELOPER"<<std::endl
			  <<" Florian Oetke"<<std::endl<<std::endl;

			ss<<"DEVELOPER"<<std::endl
			  <<" Sebastian Schalow"<<std::endl<<std::endl;

			ss<<"MUSIC"<<std::endl
			  <<" soundcloud.com/maxstack"<<std::endl<<std::endl;

			ss<<"GRAPHICS"<<std::endl
			  <<" BigRookGamesdotCom"<<std::endl
			  <<" Florian Oetke"<<std::endl<<std::endl;

			ss<<"SOUND EFFECTS"<<std::endl
			  <<" Mark McCorkle"<<std::endl
			  <<"    8-bit Platformer SFX"<<std::endl
			  <<"    for OpenGameArt.org"<<std::endl
			  <<" Jim Rogers"<<std::endl
			  <<" Mike Koenig"<<std::endl<<std::endl;

			return ss.str();
		}
	}

	Main_menu_screen::Main_menu_screen(Game_engine& game_engine, bool ingame)
	    : Screen(game_engine),
	      Ui_container(game_engine, calculate_vscreen(game_engine, 512),
	                    - vec2{
	                       300,
                   #ifndef __EMSCRIPTEN__
	                       (100*3+5*4)
	               #else
	                       (ingame ? 205 : 100)
	               #endif
	                   }/2.f,
	                   vec2{150,0},
	                   gui::vertical(5)),
	      _ingame(ingame),
	      _screen_size(calculate_vscreen(game_engine, 512)),
	      _on_quit_slot(&Main_menu_screen::_on_quit, this),
	      _background(game_engine.assets().load<Texture>("tex:ui_intro"_aid)),
	      _circle(game_engine.assets().load<Texture>("tex:ui_intro_circle"_aid)),
	      _credits(game_engine.assets().load<Font>("font:menu_font"_aid)),
	      _highscore(game_engine.assets().load<Font>("font:menu_font"_aid))
	{

		_credits.set(load_credits(game_engine.assets()), false);
		_highscore.set(load_highscore(game_engine.assets()), false);


		if(ingame || Game_screen::save_exists(game_engine)) {
			_root.add_new<gui::Button>("Continue", [ingame, &game_engine] {
				if(ingame) {
					DEBUG("=> game (continue)");
					game_engine.leave_screen();

				} else {
					try {
						game_engine.enter_screen<Game_screen>();

					} catch(util::Error&) { // load failed:
						game_engine.enter_screen<Game_screen>("rodney");
					}
				}
			});
		}

		_root.add_new<gui::Button>("New Game", [&game_engine]{
			game_engine.enter_screen<Game_screen>("rodney");
		});

#ifndef __EMSCRIPTEN__
		_root.add_new<gui::Button>("Quit", [&game_engine] {
			game_engine.exit();
		});
#endif
	}

	void Main_menu_screen::_on_enter(util::maybe<Screen&> prev) {
		enable();
		_on_quit_slot.connect(_game_engine.controllers().quit_events);
		_engine.audio_ctx().play_music(_engine.assets().load<audio::Music>("music:intro"_aid));
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
		            glm::rotate(glm::mat4(), _time_acc/15_s, {0.f,0.f,1.f}));

		float cred_scale = 0.25f;
		float hs_scale = 0.3f;

		if(_time_acc>2_s) {
			float cred_blend = glm::clamp((_time_acc.value()-2)/3.f, 0.f, 1.f);
			_text_renderer.draw(_credits,
			                    glm::vec2(-_screen_size.x/2 + (_credits.size().x*cred_scale)/2 + 20,
			                              -_credits.size().y*cred_scale),
			                    glm::vec4(0.4,0,0,1)*cred_blend, cred_scale);
		}

		_text_renderer.draw(_highscore,
		                    glm::vec2(+_screen_size.x/2 - (_highscore.size().x*hs_scale)/2 - 20,
		                              -_highscore.size().y*hs_scale),
		                    glm::vec4(0.4,0,0,1), hs_scale);

		draw_ui();
	}
}
