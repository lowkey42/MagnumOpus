
#include "game_screen.hpp"

#include <core/renderer/texture.hpp>
#include <core/asset/aid.hpp>

#include "sys/physics/transform_comp.hpp"
#include "sys/sprite/sprite_comp.hpp"

#include "game_state.hpp"


namespace mo {
	using namespace util;
	using namespace unit_literals;

	Game_screen::Game_screen(Game_engine& engine,
	                         std::string profile,
	                         std::vector<ecs::ETO> players,
	                         util::maybe<int> depth) :
		Screen(engine), _engine(engine),
	    _state(std::make_unique<Game_state>(engine,profile,players,depth)),
		_ui(engine),
		_player_sc_slot(&Game_screen::_on_state_change, this)
	{
		_player_sc_slot.connect(_state->state.state_change_events);
	}

	Game_screen::~Game_screen()noexcept {
	}

	void Game_screen::_on_enter(util::maybe<Screen&> prev) {
		auto& main_camera = _state->camera.main_camera();
		_engine.controllers().screen_to_world_coords([&main_camera](glm::vec2 p){
			return main_camera.screen_to_world(p);
		});
	}
	void Game_screen::_on_leave(util::maybe<Screen&> next) {
		_engine.controllers().screen_to_world_coords([](glm::vec2 p){
			return p;
		});
	}

	void Game_screen::_update(float delta_time) {
		_engine.assets().reload(); //< TODO[foe]: to do only do this on key-press (e.g. F12)

		_state->update(delta_time*second);
	}

	class Camera {};

	void Game_screen::_draw(float time) {
		auto vscreens = _state->draw();

		_ui.pre_draw();
		for(auto& screen : vscreens) {
			// TODO: draw screen.framebuffer

			for(auto& t : screen.targets)
				_ui.draw(screen.camera, *t);
		}
	}



	void Game_screen::_on_state_change(ecs::Entity& e, const sys::state::State_data& s) {
		if(&e==_state->main_player.get()) {
			if(s.s==sys::state::Entity_state::died) {
				INFO("The segfault bites. You die!");
				_state->delete_savegame();
				_engine.enter_screen<Game_screen>("default", std::vector<ecs::ETO>{}, util::just(0));
			}
		}
	}

	void Game_screen::_join(sys::controller::Controller_added_event e) {
		// TODO
	}

	void Game_screen::_unjoin(sys::controller::Controller_removed_event e) {
		// TODO
	}

	void Game_screen::_save()const {
		// TODO[foe]: write save file (level_save + ecs_save)
	}

}
