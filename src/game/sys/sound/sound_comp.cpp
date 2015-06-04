#include "sound_comp.hpp"

#include <unordered_map>
#include <core/sound/sound.hpp>
#include <game/sys/state/state_comp.hpp>

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#include <string>

namespace mo {
namespace sys {
namespace sound {

	struct Sounds_entry{
		std::string sound_name = "";
		audio::Sound_ptr ptr;
	};

	struct Sounds_map{
		std::unordered_map<sys::state::Entity_state, Sounds_entry> sounds;
	};

	struct Sound_comp_data{
		Sound_comp_data(std::unique_ptr<Sounds_map> data){
			_data = std::move(data);
		}

		~Sound_comp_data() = default;

		Sound_comp_data& operator=(Sound_comp_data&& rhs) noexcept {
			_data = std::move(rhs._data);
			return *this;
		}

		std::unique_ptr<Sounds_map> _data;
	};

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

	sf2_structDef(Sounds_entry,
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

}
}

namespace asset {
	template<>
	struct Loader<sys::sound::Sound_comp_data> {
		using RT = std::shared_ptr<sys::sound::Sound_comp_data>;

		static RT load(istream in) throw(Loading_failed);

		static void store(ostream out, const sys::sound::Sound_comp_data& asset) throw(Loading_failed);
	};

	std::shared_ptr<sys::sound::Sound_comp_data> Loader<sys::sound::Sound_comp_data>::load(istream in) throw(Loading_failed){
		auto r = std::make_unique<sys::sound::Sounds_map>();
		sf2::parseStream(in, *r);

		// Generating new Animation-Shared-Ptr and set _data-ptr to what r pointed to
		auto sc_data = std::make_shared<sys::sound::Sound_comp_data>(std::move(r));

		return sc_data;
	}

	void Loader<sys::sound::Sound_comp_data>::store(ostream out, const sys::sound::Sound_comp_data& asset) throw(Loading_failed) {

		sf2::writeStream(out, asset);
	}
}
}
