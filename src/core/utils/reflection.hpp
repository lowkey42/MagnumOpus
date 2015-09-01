/**************************************************************************\
 * provides compile & runtime type-information                            *
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

#include <string>
#include <typeinfo>
#include <type_traits>

namespace mo {
namespace util {

	extern std::string demangle(const char* name);

	template<class T>
	std::string typeName() {
		return demangle(typeid(T).name());
	}


	using Typeid_type = int32_t;

	constexpr auto notypeid = Typeid_type(0);

	struct Typeid_gen_base {
		protected:
			static auto next_id()noexcept {
				static auto idc = Typeid_type(1);
				return idc++;
			}
	};
	template<typename T>
	struct Typeid_gen : Typeid_gen_base {
		static auto id()noexcept {
			static auto i = next_id();
			return i;
		}
	};

	template<class T>
	auto typeid_of() {
		return Typeid_gen<std::decay_t<std::remove_pointer_t<std::decay_t<T>>>>::id();
	}
	template<>
	inline auto typeid_of<void>() {
		return notypeid;
	}

}
}
