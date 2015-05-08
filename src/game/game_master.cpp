#include "game_master.hpp"

#include "../core/utils/log.hpp"

#include <random>

#include <core/renderer/texture.hpp>

#include "sys/physics/transform_comp.hpp"
#include "sys/sprite/sprite_comp.hpp"

#include "core/asset/aid.hpp"
#include "core/renderer/animation.hpp"


namespace mo {
	Game_master::Game_master(Game_engine& engine, const Saved_game_state& state)
		: _level(state.stored_level.is_some() ? state.stored_level.get_or_throw() :
				 level::generate_level(engine.assets(),
									   state.seed,
									   state.current_level,
									   state.current_difficulty)) {

		auto& log_out = ::mo::util::debug(__func__, __FILE__, __LINE__);
		log_out<<"World "<<_level.width()<<"x"<<_level.height()
		       <<"\nRooms:\n";

		log_out<<" \n          ";

		for(auto y=0; y<_level.height(); y++) {
			for(auto x=0; x<_level.width(); x++) {
				switch(_level.get(x,y).type) {
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
	}

	void Game_master::spawn(Game_engine& engine, ecs::Entity_manager& em) {

		_level.foreach_room([&](const auto& room){
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

	void Game_master::update() {
		// TODO: add magic here
	}
}
