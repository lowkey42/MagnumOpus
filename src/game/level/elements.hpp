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
#include <memory>
#include <iterator>

namespace mo {
namespace level {

	enum class Element {
		neutral=0,

		fire,
		frost,
		water,
		stone,
		gas,
		lightning
		// add more?
	};
	constexpr auto element_count = static_cast<std::size_t>(Element::lightning) + 1;
	constexpr uint16_t mask(Element e) {
		return e!=Element::neutral ? 1<<(static_cast<uint16_t>(e)-1) : 0;
	}

	struct Elements;
	class Elements_iterator
	        : std::iterator<std::forward_iterator_tag, const Element> {

		public:
			Elements_iterator(const Elements& e, std::size_t i);

			auto operator*() const -> const Element&;
			auto operator->() const -> const Element*;

			auto operator++() -> Elements_iterator&;
			auto operator++(int) -> Elements_iterator;

			bool operator==(const Elements_iterator& rhs);
			bool operator!=(const Elements_iterator& rhs);

		private:
			const Elements* e;
			Element i;
	};

	struct Elements {
		uint16_t value; //< Bitmask of Element
		Elements():value(0){};
		Elements(uint16_t v):value(v){};
		Elements(Element e):value(mask(e)){};
		template<typename Range>
		Elements(const Range& r):value(0) {
			for(auto e : r)
				value|=mask(e);
        }

		operator bool()const noexcept {
			return value;
		}
		bool operator|(Element e)const noexcept {
			return value & mask(e);
		}
		void operator|=(Element e)noexcept {
			value|=mask(e);
		}

		auto without(Element e)const noexcept {
			return Elements{static_cast<uint16_t>(value & ~mask(e))};
		}

		bool operator==(Elements rhs)const noexcept {
			return value==rhs.value;
		}
		bool operator!=(Elements rhs)const noexcept {
			return !(*this==rhs);
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

		auto begin()const -> Elements_iterator {
			return {*this, 0};
		}
		auto end()const -> Elements_iterator {
			return {*this, element_count};
		}
	};

}
}

namespace std {
	template <> struct hash<mo::level::Elements> {
		size_t operator()(mo::level::Elements e)const noexcept {
			return static_cast<size_t>(e.value);
		}
	};
}

#ifdef MO_BUILD_SERIALIZER
#include <sf2/sf2.hpp>

namespace mo {
namespace level {
	sf2_enumDef(Element,
		sf2_value(fire),
		sf2_value(frost),
		sf2_value(water),
		sf2_value(stone),
		sf2_value(gas),
		sf2_value(lightning)
	)
}
}
#endif
