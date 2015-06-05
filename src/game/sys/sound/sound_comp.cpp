#include "sound_comp.hpp"

#include <unordered_map>
#include <core/sound/sound.hpp>
#include <game/sys/state/state_comp.hpp>

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#include "../../../core/ecs/serializer_impl.hpp"

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
		sf2_value(idle),
		sf2_value(walking),
		sf2_value(attacking_melee),
		sf2_value(attacking_range),
		sf2_value(interacting),
		sf2_value(taking),
		sf2_value(change_weapon),
		sf2_value(damaged),
		sf2_value(healed),
		sf2_value(dead),
		sf2_value(resurrected)
	)

}

namespace sound {

	sf2_structDef(Sound_entry,
		sf2_member(sound_name)
	)

	sf2_structDef(Sounds_map,
		sf2_member(sounds)
	)

	sf2_structDef(Sound_comp_data,
		sf2_member(_data)
	)

	struct Sound_comp::Persisted_state {
		std::string aid;
		Persisted_state(const Sound_comp& c) :
			aid(c._sc_data.aid().str()){}

	};

	sf2_structDef(Sound_comp::Persisted_state,
		sf2_member(aid)
	)

	void Sound_comp::load(ecs::Entity_state &state){
		auto s = state.read_to(Persisted_state{*this});
		_sc_data = state.asset_mgr().load<Sound_comp_data>(asset::AID(s.aid));
	}

	void Sound_comp::store(ecs::Entity_state &state){
		state.write_from(Persisted_state{*this});
	}

	Mix_Chunk* Sound_comp::get_sound(int pos) const noexcept {
		if(!_sc_data->_loaded_sounds.at(pos)){
			ERROR("Tried to access a Sound position that is not initialized -> try to load idle sound!");
			if(!_sc_data->_loaded_sounds.at(0)){
				CRASH_REPORT("No standard Sound at pos0 (idle)!");
			}
			INFO("Found standard sound for idle -> loading");
			return _sc_data->_loaded_sounds.at(0)->getSound();
		}
		DEBUG("LOADED SOUND FROM VECTOR AT POS " << pos);
		return _sc_data->_loaded_sounds.at(pos)->getSound();
	}

}
}

namespace asset {

	std::shared_ptr<sys::sound::Sound_comp_data> Loader<sys::sound::Sound_comp_data>::load(istream in) throw(Loading_failed){
		auto r = std::make_unique<sys::sound::Sounds_map>();
		sf2::parseStream(in, *r);

		std::vector<audio::Sound_ptr> ptrs;
		using estate = sys::state::Entity_state;
		int end = static_cast<int>(estate::resurrected);
		for(int i = 0; i <= end; i++){
			auto cur_iter = r->sounds.find(estate(i));
			ptrs.push_back(audio::Sound_ptr());
			if(cur_iter != r->sounds.end()){
				DEBUG("Sound at estate pos " << i << ": " << cur_iter->second.sound_name);
				cur_iter->second.ptr = in.manager().load<audio::Sound>(cur_iter->second.sound_name);
				ptrs.at(i) = cur_iter->second.ptr;
			}
		}

		// Generating new Sound-Shared-Ptr and set _sc_data-ptr to what r pointed to
		auto sc_data = std::make_shared<sys::sound::Sound_comp_data>(std::move(r), std::move(ptrs));

		return sc_data;
	}

	void Loader<sys::sound::Sound_comp_data>::store(ostream out, const sys::sound::Sound_comp_data& asset) throw(Loading_failed) {

		sf2::writeStream(out, asset);
	}
}
}
