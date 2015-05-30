
#include "game_screen.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <core/sound/sound.hpp>
#include <core/sound/music.hpp>
#include <core/sound/sound_ctx.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <core/renderer/texture.hpp>
#include <core/renderer/primitives.hpp>
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
		_player_sc_slot(&Game_screen::_on_state_change, this),
	    _join_slot(&Game_screen::_join, this),
	    _unjoin_slot(&Game_screen::_unjoin, this),
		_post_effect_obj(renderer::simple_vertex_layout,
						 renderer::create_buffer(std::vector<renderer::Simple_vertex>{
			{{0,0}, {0,1}},
			{{0,1}, {0,0}},
			{{1,0}, {1,1}},

			{{1,1}, {1,0}},
			{{1,0}, {1,1}},
			{{0,1}, {0,0}}
		}))
	{



		_player_sc_slot.connect(_state->state.state_change_events);
		_join_slot.connect(engine.controllers().join_events);
		_unjoin_slot.connect(engine.controllers().unjoin_events);

		_post_effects.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:simple"_aid))
					 .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:simple"_aid))
					 .bind_all_attribute_locations(renderer::simple_vertex_layout)
					 .build();
	}

	Game_screen::~Game_screen()noexcept {
	}

	void Game_screen::_on_enter(util::maybe<Screen&> prev) {

		sound::Sound_ptr mySound = _engine.assets().load<sound::Sound>("sound:test"_aid);
		sound::Music_ptr mainMusic = _engine.assets().load<sound::Music>("music:test"_aid);

		_engine.sound_ctx().music_volume(50);
		_engine.sound_ctx().play(mainMusic, Time(0));
		_engine.sound_ctx().play(mySound, Angle(0), Distance(0), 0);

		mainMusic.reset();
		mySound.reset();

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

		for(auto& screen : vscreens) {
			glm::mat4 vp = glm::ortho(0.f,1.f,1.f,0.f,-1.f,1.f);

			_post_effects.bind().set_uniform("VP", vp)
					.set_uniform("layer",   1.0f)
					.set_uniform("texture", 0)
					.set_uniform("model", glm::mat4())
					.set_uniform("color",   glm::vec4(1,1,1,1));
			screen.vscreen.bind();
			_post_effect_obj.draw();
		}

		_state->draw_ui();
	}



	void Game_screen::_on_state_change(ecs::Entity& e, sys::state::State_data& s) {
		if(&e==_state->main_player.get()) {
			if(s.s==sys::state::Entity_state::dead) {
				INFO("The segfault bites. You die!");
				_state->delete_savegame();
				_engine.enter_screen<Game_screen>("default", std::vector<ecs::ETO>{}, util::just(0));
			}
		}
	}

	void Game_screen::_join(sys::controller::Controller_added_event e) {
		// TODO
		_state->add_player(e.controller, _state->main_player->get<sys::physics::Transform_comp>().get_or_throw().position());
	}

	void Game_screen::_unjoin(sys::controller::Controller_removed_event e) {
		// TODO
	}

	void Game_screen::_save()const {
		// TODO[foe]: write save file (level_save + ecs_save)
	}

}
