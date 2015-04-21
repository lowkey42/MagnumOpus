#include "game_master.hpp"

#include "../core/utils/log.hpp"

#include <random>

namespace game {
	Game_master::Game_master(Game_engine& engine, const Saved_game_state& state)
		: _level(state.stored_level.is_some() ? state.stored_level.get_or_throw() :
				 level::generate_level(engine.assets(),
									   state.seed,
									   state.current_level,
									   state.current_difficulty)) {

		auto& log_out = ::core::util::debug(__func__, __FILE__, __LINE__);
		log_out<<"World "<<_level.width()<<"x"<<_level.height()<<" \n          ";

		for(auto y=0; y<_level.height(); y++) {
			for(auto x=0; x<_level.width(); x++) {
				switch(_level.get(x,y).type) {
					case game::level::Tile_type::wall_tile:
						log_out<<"#";
						break;
					case game::level::Tile_type::floor_tile:
						log_out<<".";
						break;
					case game::level::Tile_type::door_closed_ns:
					case game::level::Tile_type::door_closed_we:
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

	void Game_master::update() {
		// TODO: add magic here
	}
}
