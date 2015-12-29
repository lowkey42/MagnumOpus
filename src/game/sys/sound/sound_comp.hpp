/**************************************************************************\
 *	sound_comp.hpp	- Component class for Sounds                          *
 *                                                ___                     *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <core/ecs/ecs.hpp>
#include <core/asset/asset_manager.hpp>

#include <core/audio/sound.hpp>
#include <core/audio/audio_ctx.hpp>

namespace mo {
namespace sys {
namespace sound {

	struct Sound_entry;

	struct Sounds_map;

	struct Sound_comp_data {

		Sound_comp_data(std::unique_ptr<Sounds_map> data, std::vector<audio::Sound_ptr>);
		~Sound_comp_data();

		Sound_comp_data& operator=(Sound_comp_data&& rhs) noexcept;

		// Attributes
		std::unique_ptr<Sounds_map> _data;
		// TODO [Sebastian]: Absprache mit Flo -> Umwandlung in einen std::vector<std::shared_ptr<const audio::Sound>> ?
		std::vector<audio::Sound_ptr> _loaded_sounds;

	};

	class Sound_comp : public ecs::Component<Sound_comp> {

	public:

		static constexpr const char* name() {return "Sound";}
		void load(sf2::JsonDeserializer& state,
		          asset::Asset_manager& asset_mgr)override;
		void save(sf2::JsonSerializer& state)const override;

		// TODO: nullptr check
		Sound_comp(ecs::Entity& owner, asset::Ptr<Sound_comp_data> sc_data = asset::Ptr<Sound_comp_data>()) :
			Component(owner), _sc_data(sc_data){}

		std::shared_ptr<const audio::Sound> get_sound(int pos) const noexcept;

	private:
		friend class Sound_system;

		audio::Channel_id _channel = -1;
		asset::Ptr<Sound_comp_data> _sc_data;

		audio::Sound_ptr _effect;
		audio::Channel_id _effect_channel = -1;
		Time _effect_left = Time(0);

	};

}
}

namespace asset {
	template<>
	struct Loader<sys::sound::Sound_comp_data> {
		using RT = std::shared_ptr<sys::sound::Sound_comp_data>;

		static RT load(istream in) throw(Loading_failed);

		static void store(ostream out, const sys::sound::Sound_comp_data& asset) throw(Loading_failed);
	};

}
}
