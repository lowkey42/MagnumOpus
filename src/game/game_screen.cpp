
#include "game_screen.hpp"

#include <core/audio/sound.hpp>
#include <core/audio/music.hpp>
#include <core/audio/audio_ctx.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <core/renderer/texture.hpp>
#include <core/renderer/primitives.hpp>
#include <core/asset/aid.hpp>

#include "sys/physics/transform_comp.hpp"
#include "sys/graphic/sprite_comp.hpp"
#include "sys/sound/sound_comp.hpp"

#include "sys/combat/comp/score_comp.hpp"

#include "game_state.hpp"
#include "highscore.hpp"

namespace mo {
	using namespace util;
	using namespace unit_literals;

	namespace {
		std::vector<renderer::Simple_vertex> posteffect_buffer {
			{{0,0}, {0,1}},
			{{0,1}, {0,0}},
			{{1,0}, {1,1}},

			{{1,1}, {1,0}},
			{{1,0}, {1,1}},
			{{0,1}, {0,0}}
		};
	}


	bool Game_screen::save_exists(Game_engine& engine) {
		return Game_state::save_exists(engine);
	}

	Game_screen::Game_screen(Game_engine& engine)
	    : Game_screen(engine, Game_state::load(engine))
	{}

	Game_screen::Game_screen(Game_engine& engine,
							 std::string name)
	    : Game_screen(engine, Game_state::create(engine, std::move(name)))
	{}

	Game_screen::Game_screen(Game_engine& engine,
							 Profile_data profile,
							 std::vector<ecs::ETO> players,
							 int depth)
	    : Game_screen(engine, Game_state::create(engine,profile,players,depth))
	{}

	Game_screen::Game_screen(Game_engine& engine,
	                         std::unique_ptr<Game_state> state)
	    : Screen(engine), _engine(engine),
	    _state(std::move(state)),
		_player_sc_slot(&Game_screen::_on_state_change, this),
		_join_slot(&Game_screen::_join, this),
		_unjoin_slot(&Game_screen::_unjoin, this),
		_post_effect_obj(renderer::simple_vertex_layout,
						 renderer::create_buffer(posteffect_buffer))
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

	Game_screen::Game_screen(Game_engine& engine, const Saveable_state& save_file) :
	    Screen(engine), _engine(engine),
	    _state(Game_state::create_from_save(engine, save_file)),
		_player_sc_slot(&Game_screen::_on_state_change, this),
	    _join_slot(&Game_screen::_join, this),
	    _unjoin_slot(&Game_screen::_unjoin, this),
		_post_effect_obj(renderer::simple_vertex_layout,
						 renderer::create_buffer(posteffect_buffer))
	{
		_player_sc_slot.connect(_state->state.state_change_events);
		_join_slot.connect(engine.controllers().join_events);
		_unjoin_slot.connect(engine.controllers().unjoin_events);

		_post_effects.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:simple"_aid))
					 .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:simple"_aid))
					 .bind_all_attribute_locations(renderer::simple_vertex_layout)
					 .build();
	}
	auto Game_screen::save() -> Saveable_state {
		return _state->save_to();
	}

	void Game_screen::_on_enter(util::maybe<Screen&> prev) {

		audio::Music_ptr mainMusic = _engine.assets().load<audio::Music>("music:test"_aid);

		_engine.audio_ctx().play_music(mainMusic);

		auto& main_camera = _state->camera.main_camera();
		_engine.controllers().screen_to_world_coords([&main_camera](glm::vec2 p){
			return main_camera.screen_to_world(p);
		});

	}
	void Game_screen::_on_leave(util::maybe<Screen&> next) {
		_state->save();

		_engine.controllers().screen_to_world_coords([](glm::vec2 p){
			return p;
		});
		_engine.audio_ctx().stop_music();
	}

	void Game_screen::_update(float delta_time) {
		_state->update(delta_time*second);
	}

	class Camera {};

	void Game_screen::_draw(float time) {
		auto vscreens = _state->draw(Time(time));

		for(auto& screen : vscreens) {
			glm::mat4 vp = glm::ortho(0.f,1.f,1.f,0.f,-1.f,1.f);

			_post_effects.bind().set_uniform("VP", vp)
					.set_uniform("layer",   1.0f)
					.set_uniform("texture", 0)
					.set_uniform("model", glm::mat4())
					.set_uniform("color", glm::vec4(1,1,1,1));
			screen.vscreen.bind();
			_post_effect_obj.draw();
		}

		_state->draw_ui();
	}



	void Game_screen::_on_state_change(ecs::Entity& e, sys::state::State_data& s) {
		if(&e==_state->main_player.get()) {
			if(s.s==sys::state::Entity_state::dead) {
				auto score = e.get<sys::combat::Score_comp>().process(0, [](auto& s){return s.value();});
				add_score(_engine.assets(), Score{_state->profile.name, score, _state->profile.depth,
				                                  _state->profile.seed});

				INFO("The segfault bites. You die!");
				_state->delete_save();
				_engine.enter_screen<Game_screen>("default");
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

}
