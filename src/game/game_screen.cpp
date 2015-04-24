
#include "game_screen.hpp"

#include "game_master.hpp"

#include "sys/physics/transform_comp.hpp"
#include "sys/sprite/sprite_comp.hpp"



namespace game {
	using namespace core::util;
	using namespace core::unit_literals;

	constexpr auto MinEntitySize = 25_cm;
	constexpr auto MaxEntitySize = 5_m;
	constexpr auto MaxEntityVelocity = 180_km/hour;


	Meta_system::Meta_system(core::Engine& engine, level::Level& level)
		: em(), entity_store(em, engine.assets()),
	      transform(em, MaxEntitySize, level.width(), level.height()),
		  physics(em, transform, MinEntitySize, MaxEntityVelocity, level),
		  spritesys(em, transform, engine.assets()),
		  controller(em) {
	}

	void Meta_system::update(core::Time dt) {
		em.process_queued_actions();

		controller.update(dt);
		transform.update(dt);
		physics.update(dt);
	}
	void Meta_system::draw(const core::renderer::Camera& cam) {
		// TODO: draw systems here
		spritesys.draw(cam);
	}

	namespace {
		Saved_game_state load_save_game(Game_engine& engine) {
			// TODO[foe]: load

			return Saved_game_state{42, 0, 0, nothing()};
		}
	}

	Game_screen::Game_screen(Game_engine& engine) :
		core::Screen(engine), _engine(engine),
		_gm(new Game_master(engine, load_save_game(engine))),
		_state(engine, _gm->level()),
	    _camera(engine, 16.f), _tilemap(engine, _gm->level())
	{
		_camera.position({10,10});
		_add_player(_engine.controllers().main_controller());

		// TODO[foe]: remove debug code
		for(int i=0; i<8; i++) {
			core::ecs::Entity_ptr p = _state.entity_store.apply("blueprint:bullet"_aid, _state.em.emplace());

			p->get<sys::physics::Transform_comp>().get_or_throw().position({5, 2+ 1.5*i});

			float x = 32.0 / 255.0, y = 32.0 / 128.0;

			p->emplace<sys::sprite::Sprite_comp>("tex:tilemap", glm::vec4(0.0f, 1.0f, x, 1.0-y));
		}
		// END TODO
	}

	Game_screen::~Game_screen()noexcept {
		_save();
	}

	void Game_screen::_update(float delta_time) {
		_engine.assets().reload(); //< TODO[foe]: to do only do this on key-press (e.g. F12)

		_gm->update();

		_state.update(delta_time*second);
	}

	class Camera {};

	void Game_screen::_draw(float time) {
		_tilemap.draw(_camera);
		_state.draw(_camera);

		// TODO: draw ui
	}


	auto Game_screen::_add_player(sys::controller::Controller& controller) -> core::ecs::Entity_ptr {
		core::ecs::Entity_ptr p = _state.entity_store.apply("blueprint:player"_aid, _state.em.emplace());

		p->emplace<sys::controller::Controllable_comp>(&controller);

		p->get<sys::physics::Transform_comp>().process([&](sys::physics::Transform_comp& trans) {
					trans.position({10, 10});
				});

		float x = 16.0 / 255.0, y = 16.0 / 128.0;

		p->emplace<sys::sprite::Sprite_comp>("tex:tilemap", glm::vec4(0.0f, 1.0f, x, 1.0-y));

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
