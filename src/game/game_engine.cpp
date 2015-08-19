
#include "game_engine.hpp"

#include "game_screen.hpp"
#include "intro_screen.hpp"
#include "game_state.hpp"

#include "highscore.hpp"

#include <vector>
#include <algorithm>

namespace mo {

	Game_engine::Game_engine(const std::string& title, int argc, char** argv, char** env, bool start_game)
		: Engine(title, argc, argv, env),
		  _controllers(assets(), input()) {

		auto args = std::vector<std::string>();
		args.reserve(argc);
		for(int i=0; i<argc; ++i) {
			args.emplace_back(argv[i]);
		}

		if(std::find(args.begin(), args.end(), "--reload")!=args.end()) {
			std::cerr<<"Reload continue"<<std::endl;
			auto s = assets().load<Saveable_state>("cfg:debug_state"_aid);

			enter_screen<Game_screen>(*s);

		} else
			enter_screen<Intro_screen>();
	}

	Game_engine::~Game_engine() {
		leave_screen(255);
	}

	auto Game_engine::_on_reload() -> std::tuple<bool, std::string> {
		auto* screen = &current_screen();
		Game_screen* gs = dynamic_cast<Game_screen*>(screen);

		if(gs)
			assets().save("cfg:debug_state"_aid, gs->save());

		std::cerr<<"Reload "<<(gs?"started":"cancled")<<std::endl;

		return std::make_tuple(gs!=nullptr, "--reload");
	}
	void Game_engine::_on_frame(float dt) {
		static float dt_acc = 0;
		_controllers.update(Time(dt));

		if(((uint32_t)dt_acc) % 60*5 == 0) {
			prepare_list_scores();
			dt_acc+=1;
		}

		dt_acc+=dt;
	}

}
