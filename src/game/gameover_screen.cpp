#include "gameover_screen.hpp"

#include "main_menu_screen.hpp"

#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_screen.hpp"

#include <core/gui/button.hpp>
#include <core/gui/input.hpp>
#include <core/audio/music.hpp>

#include"highscore.hpp"

namespace mo {
	using namespace unit_literals;
	using namespace renderer;
	using namespace glm;

	namespace {
		bool highscore_reached(float score, std::vector<Score> scores) {
			if(scores.size()<15)
				return true;

			for(auto& s : scores) {
				if(s.score<score) {
					return true;
				}
			}

			return false;
		}

		constexpr auto continue_delay = 9_s;
	}

	Gameover_screen::Gameover_screen(Game_engine& game_engine, Score score)
	    : Screen(game_engine),
	      Ui_container(game_engine, calculate_vscreen(game_engine, 512),
	                   vec2{-300,-240}/2.f,
	                   vec2{300,0},
	                   gui::vertical(0)),
	      _background(game_engine.assets().load<Texture>("tex:ui_intro"_aid)),
	      _circle(game_engine.assets().load<Texture>("tex:ui_intro_circle"_aid)),
	      _message(create_text("GAME OVER")),
	      _hint(_font),
	      _new_highscore(_font),
	      _score(score)
	{
		std::vector<Score> scores = list_scores(game_engine.assets());

		_highscore_reached = highscore_reached(score.score, scores);


		if(_highscore_reached) {
			_hint.set("Enter your name and press return");

			_root.add_new<gui::Input>([&](auto name){
				_score.name = name;
				add_score(_engine.assets(), _score);
				_quit = true;
			}, 10);

			_score.name = "???";
			scores.push_back(_score);
			std::stable_sort(std::begin(scores), std::end(scores), [](const Score& a, const Score& b){
				return a.score > b.score;
			});
			scores.resize(std::min(scores.size(), std::size_t(15)));

		} else {
			_root.add_new<gui::Button>("Skip", [&]{
				_quit = true;
			});
		}

		_new_highscore.set(print_scores(scores));
	}

	void Gameover_screen::_on_enter(util::maybe<Screen&> prev) {
		enable();
		_engine.audio_ctx().play_music(_engine.assets().load<audio::Music>("music:intro"_aid));
	}
	void Gameover_screen::_on_leave(util::maybe<Screen&> next) {
		disable();
	}

	void Gameover_screen::_update(float delta_time) {
		_time_acc+=delta_time * second;

		update_ui();

		if(_quit)
			_game_engine.enter_screen<Game_screen>("default");

		else if(!_highscore_reached) {
			auto left = continue_delay - _time_acc;
			_hint.set("Continue in " + util::to_string(int32_t(left.value())));
			if(left<1_s) {
				_game_engine.enter_screen<Game_screen>("default");
			}
		}
	}


	void Gameover_screen::_draw(float time ) {
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
		            glm::vec4{0.5f,0.15f,0.15f,0.f},
		            glm::vec4(0,0,1,1),
		            glm::vec2(_camera.viewport().w*2 *(800/2048.f), _camera.viewport().w*2 *(800/2048.f)),
		            glm::rotate(glm::mat4(), _time_acc/15_s, {0.f,0.f,1.f}));

		_text_renderer.draw(
		            _message,
		            glm::vec2(0.f, -200.f),
		            glm::vec4(1.0,0.0,0.0,1),
			        1.f);

		_text_renderer.draw(
		            _hint,
		            glm::vec2(0.f, -150.f),
		            glm::vec4(1.0,0.0,0.0,1),
			        0.5f);

		_text_renderer.draw(_new_highscore,
			                glm::vec2(0,-150),
			                glm::vec4(0.75,0,0,1), 0.2f);
		draw_ui();
	}

}
