#include "game_state.hpp"

#include "game_engine.hpp"
#include "game_screen.hpp"

#include <core/renderer/particles.hpp>

#include <core/renderer/texture.hpp>
#include <core/asset/aid.hpp>

#include "sys/physics/transform_comp.hpp"

#include "level/level_generator.hpp"

#include "sys/physics/transform_comp.hpp"

#include "tags.hpp"

namespace mo {
	using namespace util;
	using namespace unit_literals;

	namespace {
		constexpr auto MinEntitySize = 25_cm;
		constexpr auto MaxEntitySize = 5_m;
		constexpr auto MaxEntityVelocity = 180_km/hour;

		static Profile_data im_a_savegame{"default", 42,0,0};

		struct My_environment_callback : renderer::Environment_callback {
			My_environment_callback(const level::Level& level)
				: level(level) {}

			const level::Level& level;

			bool check_collision(int x, int y)noexcept override {
				return level.solid(x, y);
			}
		};

		void set_controller(ecs::Entity& p, sys::controller::Controller& controller) {
			if(p.has<sys::controller::Controllable_comp>())
				p.get<sys::controller::Controllable_comp>()
						.get_or_throw().set(controller);
			else
				p.emplace<sys::controller::Controllable_comp>(&controller);
		}
	}


	auto Game_state::create(Game_engine& engine,
	                   std::string profile_name,
			           std::vector<ecs::ETO> players,
	                   util::maybe<int> depth) -> std::unique_ptr<Game_state> {
		auto profile = im_a_savegame;
		if(profile.seed==42)
			profile.seed = time(0);

		auto d = depth.get_or_other(profile.depth);

		bool up = d<profile.depth;

		profile.depth = d;

		// TODO[foe]: save profile
		im_a_savegame = profile;

		auto state = std::unique_ptr<Game_state>(new Game_state(engine, depth.get_or_other(profile.depth)));


		auto room_m = state->level.find_room(up ? level::Room_type::end :
		                                          level::Room_type::start);

		INVARIANT(room_m.is_some(), "Generated room has no exit/entry-point!?");
		auto room = room_m.get_or_throw();

		auto start_position = room.center() *1_m
		                      + Position{1_m,0_m} * (up ? -1 : 1);

		if(!players.empty()) {
			bool first = true;
			for(auto& p : players) {
				// TODO[foe]:  get controller
				if(!first)
					break;
				else
					first = false;

				auto& controller = engine.controllers().main_controller();

				state->add_player(controller,
				           start_position, state->em.serializer().import_entity(p));
			}

		} else {
			state->add_player(engine.controllers().main_controller(),
			           start_position);
		}


		//spawn:

		state->level.foreach_room([&](const auto& room){
			auto center = room.center();

			if(room.type==level::Room_type::start) {
				// TODO[foe]

			} else {
				for(int i=0; i<5; i++) {
					ecs::Entity_ptr enemy1 = state->em.emplace("blueprint:zombie"_aid);
					enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
				}
				for(int i=0; i<10; i++) {
					ecs::Entity_ptr enemy1 = state->em.emplace("blueprint:crow"_aid);
					enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
				}

				if(room.type==level::Room_type::end) {
					for(int i=0; i<3; i++) {
						ecs::Entity_ptr enemy1 = state->em.emplace("blueprint:vomit_zombie"_aid);
						enemy1->get<sys::physics::Transform_comp>().get_or_throw().position(center);
					}
				}
			}
		});

		return state;
	}
	auto Game_state::create_from_save(Game_engine& engine,
	                             const Saveable_state& s) -> std::unique_ptr<Game_state> {

		auto& stream = const_cast<asset::istream&>(s.my_stream.get_or_throw());
		auto initial_pos = stream.tellg();
		auto reset_stream = util::cleanup_later([&](){stream.seekg(initial_pos);});
		(void)reset_stream;


		auto depth = 0; // TODO: load depth

		auto state = std::unique_ptr<Game_state>(new Game_state(engine, depth));

		state->em.serializer().read(stream);

		for(auto& player : state->em.list<Player_tag_comp>()) {
			if(player.id()==0) {
				set_controller(player.owner(), engine.controllers().main_controller());
				state->main_player = player.owner_ptr();

			} else {
				// TODO: get controller
				state->sec_players.push_back(player.owner_ptr());
			}
		}

		return state;
	}

	auto Game_state::save() -> Saveable_state {
		return Saveable_state{em}; // TODO
	}

	Game_state::Game_state(Game_engine& engine, int depth)
		: engine(engine),
	      profile(im_a_savegame),
	      level(level::generate_level(engine.assets(),
									  profile.seed,
									  depth,
									  profile.difficulty)),
	      em(engine.assets()),
	      tilemap(engine, level),
	      transform(em, MaxEntitySize, level.width(), level.height(), level),
	      particle_renderer(engine.assets(), std::make_unique<My_environment_callback>(level)),
	      camera(em, engine),
		  physics(em, transform, MinEntitySize, MaxEntityVelocity, level),
		  state(em),
		  controller(em),
		  ai(em, transform, level),
		  combat(engine.assets(), em, transform, physics, state),
	      items(engine.assets(), em, physics, transform, state, particle_renderer),
		  graphics(em, transform, engine.assets(), particle_renderer, state),
	      soundsys(em, transform, engine.audio_ctx()),
		  ui(engine, em) {
		em.register_component_type<Player_tag_comp>();

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
							case level::Tile_type::stairs_down:
								log_out<<"<";
								break;
							case level::Tile_type::stairs_up:
								log_out<<">";
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
		items.update(dt);
		combat.update(dt);
		camera.update(dt);
		graphics.update(dt);
		state.update(dt);
		ui.update(dt);

		main_player->get<sys::physics::Transform_comp>().process(
			[&](auto& transform){
				auto x = static_cast<int>(transform.position().x.value()+0.5f);
				auto y = static_cast<int>(transform.position().y.value()+0.5f);

				auto& tile = level.get(x,y);
				if(tile.type==level::Tile_type::stairs_down) {
					/*if(this->profile.depth>=2) {
						INFO("You delved too greedily and too deep");
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "You have reached the bottom", "You delved too greedily and too deep", nullptr);
						this->engine.leave_screen();

					} else*/
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

	auto Game_state::draw(Time dt) -> util::cvector_range<sys::cam::VScreen> {
		camera.draw(
			[&](const renderer::Camera& cam,
				const std::vector<ecs::Entity_ptr>&) {

			tilemap.draw(cam);
			combat.draw(cam);
			graphics.draw(cam);
			soundsys.play_sounds(cam);
			particle_renderer.draw(dt, cam);
		});

		return camera.vscreens();
	}
	void Game_state::draw_ui() {
		ui.draw();
	}

	auto Game_state::add_player(sys::controller::Controller& controller,
	                            Position pos,
	                            ecs::Entity_ptr p) -> ecs::Entity_ptr {
		if(!p)
			p = em.emplace("blueprint:player"_aid);

		set_controller(*p, controller);

		p->get<sys::physics::Transform_comp>().process([&](sys::physics::Transform_comp& trans) {
			trans.position(pos);
		});

		if(!p->has<Player_tag_comp>()) {
			if(!main_player) {
				main_player = p;
				p->emplace<Player_tag_comp>(0);

			} else {
				sec_players.emplace_back(p);
				p->emplace<Player_tag_comp>(sec_players.size());
			}

		} else {
			auto& pt = p->get<Player_tag_comp>().get_or_throw();
			if(pt.id()==0)
				main_player = p;

			else {
				sec_players.emplace_back(p);
			}
		}

		return p;
	}

	namespace asset {
		auto Loader<Saveable_state>::load(istream in) throw(Loading_failed) -> std::shared_ptr<Saveable_state> {
			return std::make_shared<Saveable_state>(std::move(in));
		}

		void Loader<Saveable_state>::store(ostream out, const Saveable_state& asset) throw(Loading_failed) {
			asset.em.process([&](auto& em){em.serializer().write(out);});
		}
	}

}
