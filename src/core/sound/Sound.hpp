/**************************************************************************\
 * Sound -       													      *
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>

#include "../utils/log.hpp"
#include "../asset/asset_manager.hpp"
#include "../units.hpp"


namespace mo {
namespace sound {

	struct Sound_loading_failed : public asset::Loading_failed {
		explicit Sound_loading_failed(const std::string& msg)noexcept : Loading_failed(msg){}
	};

	class Sound{

		public:
			explicit Sound(const std::string& path) throw(Sound_loading_failed);
			explicit Sound(std::vector<uint8_t> buffer) throw(Sound_loading_failed);
			virtual ~Sound() noexcept;

			Sound& operator=(Sound&&)noexcept;
			Sound(Sound&& s) noexcept;


			// Attributes

			// Methods

		protected:
			Sound();

			Time length = Time(0);
			float _volume = 100.f;
			float _modulation = 1.0f;
			float _pitch = 1.0f;

	};

}


namespace asset {
	template<>
	struct Loader<sound::Sound> {
		using RT = std::shared_ptr<sound::Sound>;

		static RT load(istream in) throw(Loading_failed){
			return std::make_shared<sound::Sound>(in.bytes());
		}

		static void store(ostream out, const sound::Sound& asset) throw(Loading_failed) {
			// TODO
			FAIL("NOT IMPLEMENTED, YET!");
		}
	};
}
}

