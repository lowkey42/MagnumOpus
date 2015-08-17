/**************************************************************************\
 * a very very stupidly simple button                                     *
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

	class Button : public Widget {
		public:
			Button(Ui_ctx& ctx, std::string label,
			       Listerner clicked=Listerner{},
			       Listerner focused=Listerner{});

			void draw  (bool active)override;

			void on_activate  ()override;

			auto size()const noexcept -> glm::vec2 override;

		private:
			Ui_texture   _border;
			Ui_text      _label;

			Listerner    _clicked;
			Listerner    _focused;
			bool         _last_active = false;
	};


}
}
