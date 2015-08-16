
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

#include "main_menu_screen.hpp"


namespace mo {
	using namespace util;
	using namespace unit_literals;
	using namespace renderer;

	namespace {
		std::vector<Simple_vertex> posteffect_buffer {
			{{0,0}, {0,1}},
			{{0,1}, {0,0}},
			{{1,0}, {1,1}},

			{{1,1}, {1,0}},
			{{1,0}, {1,1}},
			{{0,1}, {0,0}}
		};

		auto create_framebuffer(Game_engine& engine) {
#ifdef SLOW_SYSTEM
			return Framebuffer(engine.graphics_ctx().win_width(), engine.graphics_ctx().win_height(), false);
#else
			if(engine.graphics_ctx().win_width()>1280)
				return Framebuffer(engine.graphics_ctx().win_width()/2.f, engine.graphics_ctx().win_height()/2.f, false);

			return Framebuffer(engine.graphics_ctx().win_width(), engine.graphics_ctx().win_height(), false);
#endif
		}

		constexpr Time fade_time = 1_s;
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
		_post_effect_obj(simple_vertex_layout,
						 create_buffer(posteffect_buffer)),
	    _lightmap{
				create_framebuffer(engine),
				create_framebuffer(engine)
		},
	    _on_quit_slot(&Game_screen::_on_quit, this)
	{

		_player_sc_slot.connect(_state->state.state_change_events);
		_join_slot.connect(engine.controllers().join_events);
		_unjoin_slot.connect(engine.controllers().unjoin_events);

		_post_effects.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:poste"_aid))
					 .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:poste"_aid))
					 .bind_all_attribute_locations(renderer::simple_vertex_layout)
					 .build();

		_lightmap_filter.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:poste"_aid))
		                .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:lmf"_aid))
		                .bind_all_attribute_locations(renderer::simple_vertex_layout)
		                .build();

		_blur_filter.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:poste"_aid))
		            .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:blur"_aid))
		            .bind_all_attribute_locations(renderer::simple_vertex_layout)
		            .build();

		_fadein_left = fade_time;
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
						 renderer::create_buffer(posteffect_buffer)),
	    _lightmap{
				create_framebuffer(engine),
				create_framebuffer(engine)
		},
	    _on_quit_slot(&Game_screen::_on_quit, this)
	{
		_on_quit_slot.connect(engine.controllers().quit_events);

		_player_sc_slot.connect(_state->state.state_change_events);
		_join_slot.connect(engine.controllers().join_events);
		_unjoin_slot.connect(engine.controllers().unjoin_events);

		_post_effects.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:poste"_aid))
		             .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:poste"_aid))
		             .bind_all_attribute_locations(renderer::simple_vertex_layout)
		             .build();

		_lightmap_filter.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:poste"_aid))
		                .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:lmf"_aid))
		                .bind_all_attribute_locations(renderer::simple_vertex_layout)
		                .build();

		_blur_filter.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:poste"_aid))
		            .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:blur"_aid))
		            .bind_all_attribute_locations(renderer::simple_vertex_layout)
		            .build();

		_fadein_left = fade_time;
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

		_fadein_left = fade_time;

		// evil hack: camera jumps on initialization => skip first 4 seconds
		_state->camera.reset();
		for(int i=0; i<60*4; ++i)
			_state->camera.update(1_s / 60);

		_on_quit_slot.connect(_engine.controllers().quit_events);
	}
	void Game_screen::_on_leave(util::maybe<Screen&> next) {
		_state->save();

		_engine.controllers().screen_to_world_coords([](glm::vec2 p){
			return p;
		});
		_engine.audio_ctx().stop_music();

		_on_quit_slot.disconnect(_engine.controllers().quit_events);
	}

	namespace {
		void move_level(Game_state& state, int offset) {
			auto players = std::vector<ecs::ETO>{
				state.em.serializer().export_entity(*state.main_player)
			};

			for(auto& p : state.sec_players) {
				players.push_back(state.em.serializer().export_entity(*p));
			}

			state.engine.enter_screen<Game_screen>(
			            state.profile,
			            players,
			            state.profile.depth+offset);
		}
	}

	void Game_screen::_update(float delta_time) {
		if(_quit_to_menu) {
			DEBUG("=> menu");
			_quit_to_menu = false;
			_engine.enter_screen<Main_menu_screen>(true);
			return;
		}

		if(_fadein_left>0_s) {

			_fadein_left-=delta_time*second;

			auto left = (fade_time - _fadein_left) / fade_time;


			if(_moving_down || _dying)
				left = 1.f-left;

			delta_time *= left/2;

			if(_fadein_left<=0_s) {
				_fadein_left = 0_s;
				if(_moving_down) {
					move_level(*_state, 1);
					return;

				} else if(_dying) {
					_engine.enter_screen<Game_screen>("default");
					return;
				}
			}

		}

		_state->main_player->get<sys::physics::Transform_comp>().process(
			[&](auto& transform){
				auto x = static_cast<int>(transform.position().x.value()+0.5f);
				auto y = static_cast<int>(transform.position().y.value()+0.5f);

				auto& tile = _state->level.get(x,y);
				if(tile.type==level::Tile_type::stairs_down) {
					if(!this->_moving_down) {
						this->_moving_down = true;
						this->_fadein_left = fade_time;
					}
				}
		});

		_state->update(delta_time*second);
	}

	class Camera {};

	void Game_screen::_draw(float time) {
		auto vscreens = _state->draw(Time(time));

		for(auto& screen : vscreens) {
			glm::mat4 vp = glm::ortho(0.f,1.f,1.f,0.f,-1.f,1.f);

			_lightmap_filter.bind().set_uniform("VP", vp)
					.set_uniform("texture", 0);
			screen.vscreen.bind();

			_lightmap[0].bind_target();
			_lightmap[0].set_viewport();
			_lightmap[0].clear();
			_post_effect_obj.draw();
			_lightmap[0].unbind_target();

			// blur _lightmap
#ifdef SLOW_SYSTEM
			constexpr int blur_iterations = 3;
#else
			constexpr int blur_iterations = 9;
#endif
			_blur_filter.bind().set_uniform("VP", vp)
					.set_uniform("texture", 0);

			int lidx = 0;
			for(int i=0; i<blur_iterations; ++i) {
				lidx = lidx==1 ? 0 : 1;

				_blur_filter.bind().set_uniform("horiz", lidx==0);

				_lightmap[lidx].bind_target();
				_lightmap[lidx].set_viewport();
				_lightmap[lidx].clear();
				_lightmap[lidx==1 ? 0 : 1].bind();
				_post_effect_obj.draw();
				_lightmap[lidx].unbind_target();
			}

			_engine.graphics_ctx().reset_viewport();

			auto fade = std::abs(_fadein_left/fade_time);

			if(_moving_down || _dying)
				fade = 1.f-fade;

			_post_effects.bind().set_uniform("VP", vp)
			        .set_uniform("fade", fade)
					.set_uniform("texture", 0)
			        .set_uniform("lightmap", 1);
			screen.vscreen.bind();
			_lightmap[lidx].bind(1);
			_post_effect_obj.draw();

			_lightmap[lidx].unbind(1);
		}

		// if engine.controllers().player_ready() => show message

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
				_dying = true;
				this->_fadein_left = fade_time*3.f;
				_engine.audio_ctx().play_static(*_engine.assets().load<audio::Sound>("sound:player_dying"_aid));
			}
		}
	}

	void Game_screen::_join(sys::controller::Controller_added_event e) {
		_state->add_player(e.controller, _state->main_player->get<sys::physics::Transform_comp>().get_or_throw().position());
	}

	void Game_screen::_unjoin(sys::controller::Controller_removed_event e) {
		_state->remove_player(e.controller);
	}
}
