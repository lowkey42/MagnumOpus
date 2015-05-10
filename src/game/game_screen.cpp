
#include "game_screen.hpp"

#include "game_master.hpp"

#include <core/renderer/texture.hpp>
#include <core/asset/aid.hpp>

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
		  controller(em, transform),
		  ai(em, engine, transform),
		  combat(em, transform, physics),
		  state(em) {
	}

	void Meta_system::update(Time dt) {
		em.process_queued_actions();

		ai.update(dt);
		controller.update(dt);
		transform.update(dt);
		physics.update(dt);
		combat.update(dt);
		camera.update(dt);
		state.update(dt);
		// TODO: update sprites and tilemap
	}
	void Meta_system::draw() {

		for(auto& cam : camera.cameras()) {
			tilemap.draw(cam.camera);
			// TODO: draw systems here
			spritesys.draw(cam.camera);
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
		_state(engine, _gm->level()),
		_ui(engine),
		_player_sc_slot(&Game_screen::_on_state_change, this)
	{
		_player_sc_slot.connect(_state.state.state_change_events);

		auto start_room_m = _gm->level().find_room(level::Room_type::start);

		INVARIANT(start_room_m.is_some(), "Generated room has no entry-point!?");
		auto start_room = start_room_m.get_or_throw();

		auto start_position = start_room.center() *1_m;

		_add_player(_engine.controllers().main_controller(), start_position);

		// TODO[foe]: remove debug code
		ecs::Entity_ptr enemy1 = _state.em.emplace("blueprint:enemy"_aid);

		enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(start_position + Position(4,2));
		// END TODO

		_state.em.erase(enemy1);

		_gm->spawn(engine, _state.em);
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

		_ui.pre_draw();
		for(auto& cam : _state.camera.cameras()) {
			for(auto& t : cam.targets)
				_ui.draw(cam.camera, *t);
		}
	}


	auto Game_screen::_add_player(sys::controller::Controller& controller, Position pos) -> ecs::Entity_ptr {
		ecs::Entity_ptr p = _state.em.emplace("blueprint:player"_aid);

		p->emplace<sys::controller::Controllable_comp>(&controller);

		p->get<sys::physics::Transform_comp>().process([&](sys::physics::Transform_comp& trans) {
					trans.position(pos);
				});

		auto anim = _engine.assets().load<renderer::Animation>("anim:player"_aid);
		p->emplace<sys::sprite::Sprite_comp>(anim);

		if(!_main_player)
			_main_player = p;
		else
			_sec_players.emplace_back(p);

		return p;
	}

	void Game_screen::_on_state_change(ecs::Entity& e, const sys::state::State_data& s) {
		if(&e==_main_player.get()) {
			if(s.s==sys::state::Entity_state::died) {
				INFO("The segfault bites. You die!");
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
