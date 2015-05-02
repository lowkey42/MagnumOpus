
#include "game_screen.hpp"

#include "game_master.hpp"

#include <core/renderer/texture.hpp>

#include "sys/physics/transform_comp.hpp"
#include "sys/sprite/sprite_comp.hpp"



namespace mo {
	using namespace util;
	using namespace unit_literals;

	constexpr auto MinEntitySize = 25_cm;
	constexpr auto MaxEntitySize = 5_m;
	constexpr auto MaxEntityVelocity = 180_km/hour;


	Meta_system::Meta_system(Game_engine& engine, level::Level& level)
		: em(engine.assets()),
	      tilemap(engine, level),
	      transform(em, MaxEntitySize, level.width(), level.height()),
	      camera(em, engine),
		  physics(em, transform, MinEntitySize, MaxEntityVelocity, level),
		  spritesys(em, transform, engine.assets()),
		  controller(em),
		  ai(em, engine, transform),
		  combat(em, engine.assets()) {
	}

	void Meta_system::update(Time dt) {
		em.process_queued_actions();

		ai.update(dt);
		controller.update(dt);
		combat.update(dt);
		transform.update(dt);
		physics.update(dt);
		camera.update(dt);
	}
	void Meta_system::draw() {

		for(auto& cam : camera.cameras()) {
			tilemap.draw(cam);
			// TODO: draw systems here
			spritesys.draw(cam);
		}
	}

	namespace {
		Saved_game_state load_save_game(Game_engine& engine) {
			// TODO[foe]: load

			return Saved_game_state{42, 0, 0, nothing()};
		}
	}

	Game_screen::Game_screen(Game_engine& engine) :
		Screen(engine), _engine(engine),
		_gm(new Game_master(engine, load_save_game(engine))),
		_state(engine, _gm->level())
	{
		auto start_room_m = _gm->level().find_room(level::Room_type::start);

		INVARIANT(start_room_m.is_some(), "Generated room has no entry-point!?");
		auto start_room = start_room_m.get_or_throw();

		auto start_position = start_room.center() *1_m;

		_add_player(_engine.controllers().main_controller(), start_position);

		// TODO[foe]: remove debug code
		ecs::Entity_ptr enemy1 = _state.em.emplace("blueprint:enemy"_aid);

		enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(start_position + Position(4,2));

		float x_enemy = 64.0f / 256.0f, y_enemy = 64.0f / 64.0f;

		auto tex = _engine.assets().load<renderer::Texture>("tex:enemy_moving"_aid);
		enemy1->emplace<sys::sprite::Sprite_comp>(tex, glm::vec4(0.0f, 1.0f, x_enemy, 1.0-y_enemy));


		// TODO[foe]: remove debug code
		ecs::Entity_ptr enemy2 = _state.em.emplace("blueprint:enemy"_aid);

		enemy2->get<sys::physics::Transform_comp>().get_or_throw().position(start_position + Position(0,2));

		auto tex2 = _engine.assets().load<renderer::Texture>("tex:tilemap_m"_aid);
		enemy2->emplace<sys::sprite::Sprite_comp>(tex2, glm::vec4(0.0f, 1.0f, x_enemy, 1.0-y_enemy));


		// TODO[foe]: remove debug code
		ecs::Entity_ptr enemy3 = _state.em.emplace("blueprint:enemy"_aid);

		enemy3->get<sys::physics::Transform_comp>().get_or_throw().position(start_position + Position(-4,0));

		float x_enemy2 = 64.0f / 512.0f, y_enemy2 = 64.0f / 64.0f;

		auto tex3 = _engine.assets().load<renderer::Texture>("tex:enemy2_moving"_aid);
		enemy3->emplace<sys::sprite::Sprite_comp>(tex3, glm::vec4(0.0f, 1.0f, x_enemy2, 1.0-y_enemy2));
		// END TODO
	}

	Game_screen::~Game_screen()noexcept {
		_save();
		_engine.controllers().screen_to_world_coords([](glm::vec2 p){
			return p;
		});
	}

	void Game_screen::_on_enter(util::maybe<Screen&> prev) {
		auto& main_camera = _state.camera.main_camera();
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

		_gm->update();

		_state.update(delta_time*second);
	}

	class Camera {};

	void Game_screen::_draw(float time) {
		_state.draw();

		// TODO: draw ui
		/*
		 *
	auto _font = assets().load<renderer::Font>("font:test"_aid);
	renderer::Shader_program _text_shader;
	_text_shader.attach_shader(assets().load<core::renderer::Shader>(("vert_shader:simple"_aid)))
		 .attach_shader(assets().load<core::renderer::Shader>(("frag_shader:simple"_aid)))
		 .bind_all_attribute_locations(renderer::text_vertex_layout)
		 .build();

	_font->bind();
	_text_shader.bind()
				.set_uniform("model",   glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f)))
				.set_uniform("VP",      glm::ortho(-1000.f,1000.f,1000.f,-1000.f))
				.set_uniform("texture", 0)
				.set_uniform("layer",   0.9f)
				.set_uniform("color",   glm::vec4(1,1,1,0.6));
	_font->text("FRAMES: "+util::to_string(delta_time))->draw();
	*/
	}


	auto Game_screen::_add_player(sys::controller::Controller& controller, Position pos) -> ecs::Entity_ptr {
		ecs::Entity_ptr p = _state.em.emplace("blueprint:player"_aid);

		p->emplace<sys::controller::Controllable_comp>(&controller);

		p->get<sys::physics::Transform_comp>().process([&](sys::physics::Transform_comp& trans) {
					trans.position(pos);
				});

		float x = 64.0f / 512.0f, y = 64.0f / 64.0f;

		auto tex = _engine.assets().load<renderer::Texture>("tex:player_moving"_aid);
		p->emplace<sys::sprite::Sprite_comp>(tex, glm::vec4(0.0f, 1.0f, x, 1.0-y));

		if(!_main_player)
			_main_player = p;
		else
			_sec_players.emplace_back(p);

		return p;
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
