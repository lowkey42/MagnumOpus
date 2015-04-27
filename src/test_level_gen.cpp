
#include "core/utils/stacktrace.hpp"

#include <iostream>
#include <exception>
#include <SDL2/SDL.h>

#include "game_engine.hpp"
#include "game/level/level_generator.hpp"

using namespace mo;

int main(int argc, char** argv) {

	util::init_stacktrace(argv[0]);

	uint64_t nseed = 0;
	if(argc>1) {
		auto seed = argv[1];
		for(std::size_t i=0; seed[i]!='\0'; ++i) {
			nseed*= std::pow(10, i);
			nseed += (std::max(std::min(seed[i], '9'), '0')-'0');
		}
	}

	char* noEnv = nullptr;
	char** env = &noEnv;
	Game_engine engine("MagnumOpus", Configuration(argc, argv, env));

	auto l = level::generate_level(engine.assets(), nseed, 0, 0);

	std::cout<<"World "<<l.width()<<"x"<<l.height()<<" seed="<<nseed<<std::endl;

	for(auto y=0; y<l.height(); y++) {
		for(auto x=0; x<l.width(); x++) {
			switch(l.get(x,y).type) {
				case level::Tile_type::wall_tile:
					std::cout<<"#";
					break;
				case level::Tile_type::floor_tile:
					std::cout<<".";
					break;
				case level::Tile_type::door_closed_ns:
				case level::Tile_type::door_closed_we:
					std::cout<<"+";
					break;
				default:
					std::cout<<" ";
			}
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
	std::cout<<std::endl;

    return 0;
}
