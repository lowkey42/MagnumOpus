/**************************************************************************\
 * basic structure of the known universe                                  *
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

#include <cstdint>
#include <initializer_list>

namespace game {

	enum class Element {
		neutral=0,

		fire       = 1<<0,
		frost      = 1<<1,
		water      = 1<<2,
		stone      = 1<<3,
		explosive  = 1<<4,
		lightning  = 1<<5,
		rubber     = 1<<6,
		// add more?
	};
	struct Elements {
		uint16_t value; //< Bitmask of Element
		Elements():value(0){};
		Elements(Element e):value(static_cast<uint16_t>(e)){};
		Elements(std::initializer_list<Element> l):value(0) {
			for(auto e : l)
				value|=static_cast<uint16_t>(e);
		};

	};

	// TODO: how to map interactions?

}
