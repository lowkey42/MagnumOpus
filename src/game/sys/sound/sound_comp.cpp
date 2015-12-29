#include "sound_comp.hpp"

#include <unordered_map>
#include <core/audio/sound.hpp>
#include <game/sys/state/state_comp.hpp>

#include <sf2/sf2.hpp>

#include <string>

namespace mo {
namespace sys {
namespace sound {

	struct Sound_entry{
		std::string sound_name;
		audio::Sound_ptr ptr;
	};

	struct Sounds_map{
		std::unordered_map<sys::state::Entity_state, Sound_entry> sounds;
	};

	Sound_comp_data::Sound_comp_data(std::unique_ptr<Sounds_map> data, std::vector<audio::Sound_ptr> ptrs){
		_data = std::move(data);
		_loaded_sounds = std::move(ptrs);

	}

	Sound_comp_data::~Sound_comp_data() = default;

	Sound_comp_data& Sound_comp_data::operator=(Sound_comp_data&& rhs) noexcept {
		_data = std::move(rhs._data);
		_loaded_sounds = std::move(rhs._loaded_sounds);
		return *this;
	}

}

namespace state {

	sf2_enumDef(sys::state::Entity_state,
		idle,
		walking,
		attacking_melee,
		attacking_range,
		interacting,
		taking,
		change_weapon,
		damaged,
		healed,
		dying,
		dead,
		resurrected
	)

}

namespace sound {

	sf2_structDef(Sound_entry,
		sound_name
	)

	sf2_structDef(Sounds_map,
		sounds
	)

	sf2_structDef(Sound_comp_data,
		_data
	)

	void Sound_comp::load(sf2::JsonDeserializer& state,
	                      asset::Asset_manager& asset_mgr){
		auto aid = _sc_data ? _sc_data.aid().str() : std::string{};
		state.read_virtual(
			sf2::vmember("aid", aid)
		);
		_sc_data = asset_mgr.load<Sound_comp_data>(asset::AID(aid));
	}

	void Sound_comp::save(sf2::JsonSerializer& state)const {
		auto aid = _sc_data ? _sc_data.aid().str() : std::string{};
		state.write_virtual(
			sf2::vmember("aid", aid)
		);
	}

	std::shared_ptr<const audio::Sound> Sound_comp::get_sound(int pos) const noexcept {
		std::shared_ptr<const audio::Sound> ret;
		audio::Sound_ptr ptr = _sc_data->_loaded_sounds.at(pos);
		if(!_sc_data->_loaded_sounds.at(pos)){
			return {};
		}
		ret = std::shared_ptr<const audio::Sound> (ptr);
		return ret;
	}

}
}

namespace asset {

	std::shared_ptr<sys::sound::Sound_comp_data> Loader<sys::sound::Sound_comp_data>::load(istream in) throw(Loading_failed){
		auto r = std::make_unique<sys::sound::Sounds_map>();
		sf2::deserialize_json(in, [&](auto& msg, uint32_t row, uint32_t column) {
			ERROR("Error parsing JSON from "<<in.aid().str()<<" at "<<row<<":"<<column<<": "<<msg);
		}, *r);

		// TODO [Sebastian]: automatic calculation of enum-class ending
		using estate = sys::state::Entity_state;
		int end = static_cast<int>(estate::resurrected);

		std::vector<audio::Sound_ptr> ptrs(end, audio::Sound_ptr());

		for(int i = 0; i <= end; i++){
			auto cur_iter = r->sounds.find(estate(i));
			// if entry for entity_state(i) is given in config-file -> map it into the corresponding array position
			if(cur_iter != r->sounds.end()){
				cur_iter->second.ptr = in.manager().load<audio::Sound>(cur_iter->second.sound_name);
				ptrs.at(i) = cur_iter->second.ptr;
			}
		}

		// Generating new Sound-Shared-Ptr and set _sc_data-ptr to what r pointed to
		auto sc_data = std::make_shared<sys::sound::Sound_comp_data>(std::move(r), std::move(ptrs));

		return sc_data;
	}

	void Loader<sys::sound::Sound_comp_data>::store(ostream out, const sys::sound::Sound_comp_data& asset) throw(Loading_failed) {

		sf2::serialize_json(out, asset);
	}
}
}
