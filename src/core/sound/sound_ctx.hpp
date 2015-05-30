/**************************************************************************\
 * Sound & Music context creation + management                            *
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

#include <memory>
#include <string>
#include "sound.hpp"
#include "music.hpp"

#include "../units.hpp"

namespace mo {
	namespace asset {
		class Asset_manager;
	}

namespace sound{

	using Channel_id = int;
	class Sound_ctx {

	public:
		Sound_ctx(const std::string& name, asset::Asset_manager& assets);
		~Sound_ctx() = default;

		void play(std::shared_ptr<const sound::Music> music, Time fade_time) const noexcept;
		void play(std::shared_ptr<const sound::Sound> sound, Angle angle, Distance dist, int loop) const noexcept;
		void sound_volume(std::shared_ptr<const sound::Sound> sound, int volume) const noexcept;
		void music_volume(int volume) const noexcept;
		void stop(Channel_id) const noexcept;
		void pauseAll() const noexcept;
		void resumeAll() const noexcept;
		Channel_id set_position(Channel_id, Angle angle, Distance dist) const noexcept;

	private:
		short _sound_volume;
		short _music_volume;

	};

}
}

