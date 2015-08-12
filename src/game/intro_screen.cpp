#include "intro_screen.hpp"

#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_screen.hpp"
#include <game/example_screen.hpp>


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
					_engine.enter_screen<Game_screen>();
				else {
					INFO("no savegame");
					_engine.enter_screen<Game_screen>("TODO");
				}
			}

			Game_engine& _engine;

		};
	}

	Intro_screen::Intro_screen(Game_engine& game_engine) :
		Screen(game_engine),
		_game_engine(game_engine),
		_box(game_engine.assets(), "tex:ui_intro"_aid, 2.f, 2.f)
	{
		glm::vec2 win_size = glm::vec2(2, 2);
		_camera.reset(new renderer::Camera(win_size));
	}


	void Intro_screen::_update(float delta_time) {

		ui_controller uic(_game_engine);
		_game_engine.controllers().main_controller()(uic);

	}


	void Intro_screen::_draw(float time ) {

		_box.set_vp(_camera->vp());
		_box.draw(glm::vec2(0.f, 0.f));

	}
}
