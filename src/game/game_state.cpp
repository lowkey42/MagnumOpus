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

	static Profile_data im_a_savegame{"default", 42,0,0};

	Game_state::Game_state(Game_engine& engine,
	                       std::string profile_name,
				           std::vector<ecs::ETO> players,
				           util::maybe<int> depth)
		: engine(engine),
	      profile(im_a_savegame),
	      level(level::generate_level(engine.assets(),
									  profile.seed,
									  depth.get_or_other(profile.depth),
									  profile.difficulty)),
	      em(engine.assets()),
	      tilemap(engine, level),
	      transform(em, MaxEntitySize, level.width(), level.height(), level),
	      camera(em, engine),
		  physics(em, transform, MinEntitySize, MaxEntityVelocity, level),
		  spritesys(em, transform, engine.assets()),
		  controller(em, transform),
		  ai(em, engine, transform, level),
		  combat(em, transform, physics),
		  state(em),
		  ray_renderer(engine.assets()) {

		auto d = depth.get_or_other(profile.depth);

		auto room_m = level.find_room(d<profile.depth ? level::Room_type::end :
		                                                level::Room_type::start);

		INVARIANT(room_m.is_some(), "Generated room has no exit/entry-point!?");
		auto room = room_m.get_or_throw();

		auto start_position = room.center() *1_m
		                      + Position{1_m,0_m};
		profile.depth = d;

		if(!players.empty()) {
			bool first = true;
			for(auto& p : players) {
				if(!first)
					break;
				else
					first = false;

				auto& controller = engine.controllers().main_controller();

				add_player(controller,
				           start_position, em.serializer().import_entity(p));
			}

		} else {
			add_player(engine.controllers().main_controller(),
			           start_position);
		}

		// TODO[foe]: save profile
		im_a_savegame = profile;

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
				for(int i=0; i<5; i++) {
					ecs::Entity_ptr enemy1 = em.emplace("blueprint:zombie"_aid);
					enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
				}
				for(int i=0; i<10; i++) {
					ecs::Entity_ptr enemy1 = em.emplace("blueprint:crow"_aid);
					enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
				}

				if(room.type==level::Room_type::end) {
					for(int i=0; i<3; i++) {
						ecs::Entity_ptr enemy1 = em.emplace("blueprint:vomit_zombie"_aid);
						enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
					}
				}
			}
		});
	}

	void Game_state::delete_savegame() {
		im_a_savegame = Profile_data{"default", 42,0,0};
	}

	namespace {
		void move_level(Game_state& state, int offset) {
			auto players = std::vector<ecs::ETO>{
				state.em.serializer().export_entity(*state.main_player)
			};

			for(auto& p : state.sec_players) {
				players.push_back(state.em.serializer().export_entity(*p));
			}

			for(auto& p : players)
				DEBUG("PLAYER: "<<p);

			state.engine.enter_screen<Game_screen>(
			            state.profile.name,
			            players,
			            util::just(state.profile.depth+offset));
		}
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
					if(this->profile.depth>=2) {
						INFO("You delved too greedily and too deep");
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "You have reached the bottom", "You delved too greedily and too deep", nullptr);
						this->engine.leave_screen();

					} else
						move_level(*this, 1);

				}else if(tile.type==level::Tile_type::stairs_up) {
					if(this->profile.depth==0) {
						INFO("You have reached the top");
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "You have reached the top", "You have reached the top", nullptr);
						this->engine.leave_screen();

					} else
						move_level(*this, -1);
				}
		});
	}

	void Game_state::draw() {
		for(auto& cam : camera.cameras()) {
			tilemap.draw(cam.camera);

			ray_renderer.set_vp(cam.camera.vp());
			for(auto& p : cam.targets) {
				p->get<sys::physics::Transform_comp>().process(
					[&](sys::physics::Transform_comp& t) {
						Distance dist = 20_m;
						util::maybe<ecs::Entity&> entity = util::nothing();

						std::tie(entity, dist) =
								transform.raycast_nearest_entity(t.position(),
																 t.rotation(),
																 20_m,
																 util::justPtr(p.get()));

						auto p = remove_units(t.position());

						entity.process([&](ecs::Entity& e){
							dist = Distance(glm::length(p - remove_units(e.get<sys::physics::Transform_comp>().get_or_throw().position())));
						});

						ray_renderer.draw(glm::vec3(p.x,p.y,0.1), t.rotation(), dist.value(), 0.03);
				});
			}

			// TODO: draw systems here
			spritesys.draw(cam.camera);
		}
	}

	auto Game_state::add_player(sys::controller::Controller& controller,
	                            Position pos,
	                            ecs::Entity_ptr p) -> ecs::Entity_ptr {
		if(!p)
			p = em.emplace("blueprint:player"_aid);

		if(p->has<sys::controller::Controllable_comp>())
			p->get<sys::controller::Controllable_comp>()
			        .get_or_throw().set(controller);
		else
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
