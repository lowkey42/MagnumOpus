/**************************************************************************\
 * simple wrapper for SDL-Sounds                                          *
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

#include <string>
#include <vector>
#include <stdexcept>

#include "../utils/log.hpp"
#include "../asset/asset_manager.hpp"

typedef struct Mix_Chunk Mix_Chunk;

namespace mo {
namespace audio {

	struct Sound_loading_failed : public asset::Loading_failed {
		explicit Sound_loading_failed(const std::string& msg)noexcept : Loading_failed(msg){}
	};

	class Sound {
		public:
			Sound() = delete;
			explicit Sound(asset::istream stream) throw(Sound_loading_failed);
			virtual ~Sound()noexcept = default;

			Sound& operator=(Sound&&) noexcept = default;

			Sound(const Sound&) = delete;
			Sound& operator=(const Sound&) = delete;

			auto getSound() const noexcept { return _handle.get(); }

			bool valid()const noexcept {return _handle.get();}

		protected:
			std::unique_ptr<Mix_Chunk,void(*)(Mix_Chunk*)> _handle;

	};
	using Sound_ptr = asset::Ptr<Sound>;


} /* namespace sound */

namespace asset {
	template<>
	struct Loader<audio::Sound> {
		using RT = std::shared_ptr<audio::Sound>;

		static RT load(istream in) throw(Loading_failed){
			return std::make_unique<audio::Sound>(std::move(in));
		}

		static void store(ostream out, const audio::Sound& asset) throw(Loading_failed) {
			// TODO
			FAIL("NOT IMPLEMENTED, YET!");
		}
	};
}
}
