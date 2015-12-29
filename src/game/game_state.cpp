#include "game_state.hpp"

#include "game_engine.hpp"
#include "game_screen.hpp"

#include <core/renderer/particles.hpp>

#include <core/renderer/texture.hpp>
#include <core/asset/aid.hpp>
#include <core/utils/random.hpp>

#include <ctime>
#include <sstream>

#include "sys/physics/transform_comp.hpp"

#include "level/level_generator.hpp"

#include "sys/physics/transform_comp.hpp"

#include "tags.hpp"

#include <sf2/sf2.hpp>


namespace mo {
	using namespace util;
	using namespace unit_literals;

	namespace {
		constexpr auto MinEntitySize = 0.05_m;
		constexpr auto MaxEntitySize = 5_m;
		constexpr auto MaxEntityVelocity = 90_km/hour;

		auto rng = util::create_random_generator();

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


	auto Game_state::create(Game_engine& engine, std::string name) -> std::unique_ptr<Game_state> {
		auto profile = Profile_data{
			name,
			(uint64_t) time(0),
			0,
			0
		};

		return create(engine, profile, {}, 0);
	}
	auto Game_state::create(Game_engine& engine,
	                   Profile_data profile,
	                   std::vector<ecs::ETO> players,
	                   int depth) -> std::unique_ptr<Game_state> {

		bool up = depth<profile.depth;

		profile.depth = depth;

		auto state = std::unique_ptr<Game_state>(new Game_state(engine, profile));


		auto room_m = state->level.find_room(up ? level::Room_type::end :
		                                          level::Room_type::start);

		INVARIANT(room_m.is_some(), "Generated room has no exit/entry-point!?");
		auto room = room_m.get_or_throw();

		auto start_position = room.center() *1_m
		                      + Position{1_m,0_m} * (up ? -1 : 1);

		if(!players.empty()) {
			for(auto& p : players) {
				auto player = ecs::load_entity(state->em, p);

				player->get<sys::physics::Transform_comp>().process([&](sys::physics::Transform_comp& trans) {
					trans.position(start_position);
				});

				auto& tag = player->get<Player_tag_comp>().get_or_throw();

				if(tag.id()==0) {
					set_controller(*player, engine.controllers().main_controller());
					state->main_player = player;

				} else {
					engine.controllers().gamepad(tag.id()-1).process([&](auto& pad){
						set_controller(*player, pad);
						state->sec_players.push_back(player);

					}).on_nothing([&]{
						INFO("Dropped player "<<std::size_t(tag.id())<<" on level-switch: Too few controllers");
						state->em.erase(player);
					});
				}
			}

		} else {
			state->add_player(engine.controllers().main_controller(),
			           start_position);
		}


		//spawn:

		state->level.foreach_room([&](const auto& room){
			auto w = room.width();
			auto h = room.height();
			auto center = room.center();

			auto rand_pos = [center,w,h](){
				return center + glm::vec2{
					util::random_int(rng,-w/2+2,w/2-2),
					util::random_int(rng,-h/2+2,h/2-2)
				};
			};

			auto spawn = [&](const asset::AID& aid) {
				ecs::Entity_ptr e = state->em.emplace(aid);
				e->get<sys::physics::Transform_comp>().get_or_throw().position(rand_pos());
			};


			auto box_count = util::random_int(rng, 0, 2);
			for(int i=0; i<box_count; i++) {
				spawn("blueprint:box"_aid);
			}

			auto barrel_count = util::random_int(rng, 0, 2);
			for(int i=0; i<barrel_count; i++) {
				spawn("blueprint:barrel"_aid);
			}

			if(room.type==level::Room_type::start) {
				spawn("blueprint:box"_aid);

			} else {
				auto zombie_count = util::random_int(rng, 2, 5);
				auto crow_count   = util::random_int(rng, 0, 10);

				for(int i=0; i<zombie_count; i++) {
					spawn("blueprint:zombie"_aid);
				}
				for(int i=0; i<crow_count; i++) {
					spawn("blueprint:crow"_aid);
				}

				if(room.type==level::Room_type::end) {
					auto enemy_count = util::random_int(rng, 1, 2);

					switch(util::random_int(rng, depth>0 ? 0 : 2, 3)) {
						case 0:
							for(int i=0; i<enemy_count; i++)
								spawn("blueprint:pyro"_aid);
							break;

						case 1:
							for(int i=0; i<enemy_count; i++)
								spawn("blueprint:vomit_zombie"_aid);
							break;

						case 2:
						default:
							for(int i=0; i<enemy_count; i++)
								spawn("blueprint:turret_ice"_aid);
							break;
					}

				} else {
					if(util::random_bool(rng, 0.3)) {
						spawn("blueprint:turret_ice"_aid);
					}
				}
			}
		});

		state->save();

		return state;
	}

	auto Game_state::load(Game_engine& engine) -> std::unique_ptr<Game_state> {
		return create_from_save(engine, *engine.assets().load<Saveable_state>("cfg:savegame"_aid));
	}
	bool Game_state::save_exists(Game_engine& engine) {
		return engine.assets().exists("cfg:savegame"_aid);
	}
	void Game_state::save() {
		engine.assets().save("cfg:savegame"_aid, save_to());
	}

	auto Game_state::create_from_save(Game_engine& engine,
	                             const Saveable_state& s) -> std::unique_ptr<Game_state> {

		auto& stream = const_cast<asset::istream&>(s.my_stream.get_or_throw());
		auto initial_pos = stream.tellg();
		auto reset_stream = util::cleanup_later([&](){stream.seekg(initial_pos);});
		(void)reset_stream;


		Profile_data profile = Profile_data{};
		sf2::deserialize_json(stream, profile);

		auto state = std::unique_ptr<Game_state>(new Game_state(engine, profile));

		state->em.read(stream);

		for(auto& player : state->em.list<Player_tag_comp>()) {
			if(player.id()==0) {
				set_controller(player.owner(), engine.controllers().main_controller());
				state->main_player = player.owner_ptr();

			} else {
				engine.controllers().gamepad(player.id()-1, true).process([&](auto& pad){
					set_controller(player.owner(), pad);
					state->sec_players.push_back(player.owner_ptr());
				}).on_nothing([&]{
					INFO("Dropped player "<<std::size_t(player.id())<<" on load: Too few controllers");
					state->em.erase(player.owner_ptr());
				});
			}
		}

		if(!state->main_player) {
			state.reset();
			WARN("Corrupted savefile!");
			delete_save();
			throw util::Error("Load failed: Corrupted savefile!");
		}


		return state;
	}

	auto Game_state::save_to() -> Saveable_state {
		return Saveable_state{em, profile};
	}

	Game_state::Game_state(Game_engine& engine, Profile_data profile)
		: engine(engine),
	      profile(profile),
	      level(level::generate_level(engine.assets(),
									  profile.seed,
									  profile.depth,
									  profile.difficulty)),
	      em(engine.assets()),
	      tilemap(engine, level),
	      transform(em, MaxEntitySize, level.width(), level.height(), level),
	      particle_renderer(engine.assets(), std::make_unique<My_environment_callback>(level)),
	      forcefeedback_handler(&Game_state::forcefeedback, this),
	      camera(em, engine),
		  physics(em, transform, MinEntitySize, MaxEntityVelocity, level),
		  state(em),
		  controller(em),
		  ai(em, transform, level),
		  graphics(em, transform, engine.assets(), particle_renderer, state),
		  combat(engine.assets(), em, transform, physics, state, effect_bus, forcefeedback_bus),
	      items(engine.assets(), engine.audio_ctx(), em, physics, transform, state, particle_renderer),
	      elements(engine.assets(), em, combat),
	      soundsys(engine.assets(), em, transform, engine.audio_ctx()),
	      ui(engine, em, transform, [&](){return this->items.score_multiplicator();}) {
		em.register_component_type<Player_tag_comp>();

		graphics.effects.connect(effect_bus);
		soundsys.effects.connect(effect_bus);
		forcefeedback_handler.connect(forcefeedback_bus);

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

	void Game_state::delete_save() {
		im_a_savegame = Profile_data{"default", 42,0,0};
	}

	void Game_state::forcefeedback(Position p, float f) {
		constexpr float max_dist = 20;
		auto dist = glm::length(camera.main_camera().position()-remove_units(p));

		if(dist<=max_dist) {
			if(dist>0.25f * max_dist)
				f*=(dist/(max_dist*0.75f));

			camera.feedback(f);

			if(f<=0.15)
				f*=2;

			controller.feedback(f);
		}
	}

	float Game_state::saturation()const {
		return _screen_saturation;
	}
	void Game_state::update(Time dt) {
		em.process_queued_actions();

		auto saturation = !items.bullet_time_active() ? 1.f : main_player->get<sys::controller::Controllable_comp>().process(1.f, [](auto& p){
			if(p.active())
				return 0.4f;
			else
				return 0.0f;
		});
		_screen_saturation=glm::mix(_screen_saturation, saturation, dt.value()*5.f);


		auto wdt = !main_player || !items.bullet_time_active() ? dt : main_player->get<sys::controller::Controllable_comp>().process(dt, [dt](auto& p){
			if(p.active())
				return dt;
			else
				return dt/10.f;
		});

		ai.update(wdt);
		controller.update(dt);
		transform.update(dt);
		physics.update(wdt);
		items.update(dt, wdt);
		combat.update(wdt);
		camera.update(dt);
		graphics.update(wdt);
		soundsys.update(wdt);
		state.update(wdt);
		ui.update(dt);
		particle_renderer.update(wdt, camera.main_camera());

		camera.draw(
			[&](const renderer::Camera& cam,
				const std::vector<ecs::Entity_ptr>&) {

			soundsys.play_sounds(cam);
		});
	}

	auto Game_state::draw(Time dt) -> util::cvector_range<sys::cam::VScreen> {
		camera.draw(
			[&](const renderer::Camera& cam,
				const std::vector<ecs::Entity_ptr>&) {

			tilemap.draw(cam);
			combat.draw(cam);
			graphics.draw(cam);
			particle_renderer.draw(cam);
		});

		return camera.vscreens();
	}
	void Game_state::draw_ui() {
		ui.draw(camera.main_camera());
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
	void Game_state::remove_player(sys::controller::Controller& controller) {
		for(auto& p : sec_players) {
			p->get<sys::controller::Controllable_comp>().process([&](auto& cc){
				if(cc.is(controller))
					em.erase(p);
			});
		}
	}

	sf2_structDef(Profile_data,
		name,
		seed,
		difficulty,
		depth
	)

	namespace asset {
		auto Loader<Saveable_state>::load(istream in) throw(Loading_failed) -> std::shared_ptr<Saveable_state> {
			return std::make_shared<Saveable_state>(std::move(in));
		}

		void Loader<Saveable_state>::store(ostream out, const Saveable_state& asset) throw(Loading_failed) {
			asset.profile.process([&](auto& p){
				sf2::serialize_json(out, p);
			});
			out<<std::endl;
			asset.em.process([&](auto& em){em.write(out);});
		}
	}

}
