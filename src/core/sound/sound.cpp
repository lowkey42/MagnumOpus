#include "Sound.hpp"

namespace mo {
namespace sound {

	Sound::Sound(const std::string& path) throw(Sound_loading_failed){

	}



}

namespace asset{

	std::shared_ptr<sound::Sound> Loader<sound::Sound>::load(istream in) throw(Loading_failed){

		auto s = std::make_shared<sound::Sound>();
		// Right implementation here


	}

}
}
