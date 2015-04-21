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

		fire,
		frost,
		water,
		stone,
		explosive,
		lightning,
		rubber,
		// add more?
	};
	constexpr auto element_count = static_cast<std::size_t>(Element::rubber) + 1;
	constexpr uint16_t mask(Element e) {
		return e!=Element::neutral ? 1<<(static_cast<uint16_t>(e)-1) : 0;
	}

	struct Elements {
		uint16_t value; //< Bitmask of Element
		Elements():value(0){};
		Elements(Element e):value(mask(e)){};
		Elements(std::initializer_list<Element> l):value(0) {
			for(auto e : l)
				value|=mask(e);
		};

		operator bool()const noexcept {
			return value;
		}
		bool operator|(Element e)const noexcept {
			return value & mask(e);
		}
		void operator|=(Element e)noexcept {
			value|=mask(e);
		}

		std::size_t size()const noexcept {
			if(!*this)
				return 0;

			std::size_t c = 0;
			for(auto i=0ul; i<element_count; ++i) {
				if(value & mask(static_cast<Element>(i)))
					c++;
			}

			return c;
		}
		Element operator[](std::size_t n)const noexcept {
			if(!*this)
				return Element::neutral;

			for(auto i=0ul; i<element_count; ++i) {
				auto e = static_cast<Element>(i);

				if(value & mask(e)) {
					if(n==0)
						return e;
					else
						--n;
				}
			}

			return Element::neutral;
		}
	};

	// TODO: how to map interactions?

}
