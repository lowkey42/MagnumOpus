#include "game_state.hpp"

#include "game_engine.hpp"
#include "game_screen.hpp"


#include <core/renderer/texture.hpp>
#include <core/asset/aid.hpp>

#include "sys/physics/transform_comp.hpp"
#include "sys/sprite/sprite_comp.hpp"

#include "level/level_generator.hpp"

#include "sys/physics/transform_comp.hpp"

namespace mo {
	using namespace util;
	using namespace unit_literals;

	constexpr auto MinEntitySize = 25_cm;
	constexpr auto MaxEntitySize = 5_m;
	constexpr auto MaxEntityVelocity = 180_km/hour;


	Game_state::Game_state(Game_engine& engine,
	                       std::string profile_name,
				           std::vector<ecs::ETO> players,
				           util::maybe<int> depth)
		: engine(engine),
	      profile{42,0,0},
	      level(level::generate_level(engine.assets(),
									  profile.seed,
									  depth.get_or_other(profile.depth),
									  profile.difficulty)),
	      em(engine.assets()),
	      tilemap(engine, level),
	      transform(em, MaxEntitySize, level.width(), level.height()),
	      camera(em, engine),
		  physics(em, transform, MinEntitySize, MaxEntityVelocity, level),
		  spritesys(em, transform, engine.assets()),
		  controller(em, transform),
		  ai(em, engine, transform),
		  combat(em, transform, physics),
		  state(em) {

		depth.process([&](auto d){
			if(d<profile.depth) {
				auto start_room_m = level.find_room(level::Room_type::end);

				INVARIANT(start_room_m.is_some(), "Generated room has no exit-point!?");
				auto start_room = start_room_m.get_or_throw();

				auto start_position = start_room.center() *1_m
				                      + Position{1_m,0_m};

				add_player(engine.controllers().main_controller(),
				           start_position);


			} else {
				auto start_room_m = level.find_room(level::Room_type::start);

				INVARIANT(start_room_m.is_some(), "Generated room has no entry-point!?");
				auto start_room = start_room_m.get_or_throw();

				auto start_position = start_room.center() *1_m
				                      + Position{1_m,0_m};

				add_player(engine.controllers().main_controller(),
				           start_position);
			}

			profile.depth = d;
		});

		// TODO[foe]: save profile

// TODO[foe]: remove
		auto& log_out = ::mo::util::debug(__func__, __FILE__, __LINE__);
		log_out<<"World "<<level.width()<<"x"<<level.height()
		       <<"\nRooms:\n";

		log_out<<" \n          ";

		for(auto y=0; y<level.height(); y++) {
			for(auto x=0; x<level.width(); x++) {
				switch(level.get(x,y).type) {
					case level::Tile_type::wall_tile:
						log_out<<"#";
						break;
					case level::Tile_type::floor_tile:
						log_out<<".";
						break;
					case level::Tile_type::door_closed_ns:
					case level::Tile_type::door_closed_we:
						log_out<<"+";
						break;
					default:
						log_out<<" ";
				}
			}
			log_out<<"\n          ";
		}

		log_out<<std::endl; // end log-line
// END TODO


		//spawn:

		level.foreach_room([&](const auto& room){
			auto center = room.center();

			if(room.type==level::Room_type::start) {
				// TODO[foe]

			} else {
				for(int i=0; i<2; i++) {
					ecs::Entity_ptr enemy1 = em.emplace("blueprint:enemy"_aid);
					enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
					auto anim1 = engine.assets().load<renderer::Animation>("anim:scorpion"_aid);
					enemy1->emplace<sys::sprite::Sprite_comp>(anim1);

					ecs::Entity_ptr enemy3 = em.emplace("blueprint:enemy"_aid);
					enemy3->get<sys::physics::Transform_comp>().get_or_throw().position(center);
					auto anim2 = engine.assets().load<renderer::Animation>("anim:crawler"_aid);
					enemy3->emplace<sys::sprite::Sprite_comp>(anim2);
				}
			}
		});
	}

	void Game_state::update(Time dt) {
		em.process_queued_actions();

		ai.update(dt);
		controller.update(dt);
		transform.update(dt);
		physics.update(dt);
		combat.update(dt);
		camera.update(dt);
		state.update(dt);
		// TODO: update sprites and tilemap

		main_player->get<sys::physics::Transform_comp>().process(
			[&](auto& transform){
				auto x = static_cast<int>(transform.position().x.value()+0.5f);
				auto y = static_cast<int>(transform.position().y.value()+0.5f);

				auto& tile = level.get(x,y);
				if(tile.type==level::Tile_type::stairs_down) {
					// TODO[foe]: transfer players, too

					if(this->profile.depth>=2) {
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "You have reached the bottom", "You delved too greedily and too deep", nullptr);
						this->engine.leave_screen();
					} else

					this->engine.enter_screen<Game_screen>(
					            "default",
					            std::vector<ecs::ETO>{},
					            util::just(this->profile.depth+1));

				}else if(tile.type==level::Tile_type::stairs_up) {
					if(this->profile.depth==0) {
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "You have reached the top", "You have reached the top", nullptr);
						this->engine.leave_screen();
					} else

					this->engine.enter_screen<Game_screen>(
					            "default",
					            std::vector<ecs::ETO>{},
					            util::just(this->profile.depth-1));
				}
		});
	}
	void Game_state::draw() {

		for(auto& cam : camera.cameras()) {
			tilemap.draw(cam.camera);
			// TODO: draw systems here
			spritesys.draw(cam.camera);
		}
	}

	auto Game_state::add_player(sys::controller::Controller& controller, Position pos) -> ecs::Entity_ptr {
		ecs::Entity_ptr p = em.emplace("blueprint:player"_aid);

		p->emplace<sys::controller::Controllable_comp>(&controller);

		p->get<sys::physics::Transform_comp>().process([&](sys::physics::Transform_comp& trans) {
			trans.position(pos);
		});

		if(!main_player)
			main_player = p;
		else
			sec_players.emplace_back(p);

		return p;
	}

}
