/**************************************************************************\
 * a very very stupidly simple inputbox                                   *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include "widget.hpp"

#include "../renderer/primitives.hpp"
#include "../renderer/text.hpp"
#include "../audio/audio_ctx.hpp"


namespace mo {
namespace gui {

	using Input_listerner = std::function<void(std::string)>;

	class Input : public Widget {
		public:
			Input(Ui_ctx& ctx,
			       Input_listerner finished, int32_t max=-1,
			       Listerner focused=Listerner{});

			void draw  (bool active)override;

			void on_input(const std::string& c)override;

			auto size()const noexcept -> glm::vec2 override;

		private:
			Ui_texture   _border;
			Ui_text      _text;
			std::string  _text_str;

			Input_listerner    _finished;
			Listerner    _focused;
			int32_t      _max;
			bool         _last_active = false;
	};


}
}
