/***********************************************************\
 * commons.hpp - Defines common types and constants        *
 *     ___________ _____                                   *
 *    /  ___|  ___/ __  \                                  *
 *    \ `--.| |_  `' / /'                                  *
 *     `--. \  _|   / /                                    *
 *    /\__/ / |   ./ /___                                  *
 *    \____/\_|   \_____/                                  *
 *                                                         *
 *                                                         *
 *  Copyright (c) 2014 Florian Oetke                       *
 *                                                         *
 *  This file is part of SF2 and distributed under         *
 *  the MIT License. See LICENSE file for details.         *
\***********************************************************/

#ifndef SF2_COMMONS_HPP_
#define SF2_COMMONS_HPP_

#include <string>
#include <unordered_map>
#include <map>
#include <iostream>
#include "../io/CharSource.hpp"

namespace sf2 {

	inline bool onError(std::string error, io::CharSource& source) {
#ifndef WARN
		std::cerr<<"SF2-ParserError: \""<<error<<"\" at "<<source.line()<<":"<<source.column()<<"; nextChar="<<static_cast<int>(source())<<std::endl;
		return false;
#else
		WARN("SF2-ParserError: \""<<error<<"\" at "<<source.line()<<":"<<source.column()<<"; nextChar="<<static_cast<int>(source()));
		return false;
#endif
	}

	namespace details {
		constexpr std::size_t MAX_MEMBER_NAME_LENGTH = 64;
		constexpr std::size_t MAX_ENUM_VALUE_LENGTH = 32;

		template<typename T>
		struct ParserFunc {
			virtual ~ParserFunc() = default;
			virtual char parse(io::CharSource&, T&)const = 0;
			virtual void write(io::CharSink& sink, const T& obj)const = 0;
		};

		template<typename T>
		using ParserFuncRef = const ParserFunc<T>& (*)();

		template<typename T>
		using MemberMap = std::unordered_map<WeakStringRef, ParserFuncRef<T>, WeakStringRefHash, WeakStringRefComp>;

		template<typename T>
		using ValueNameMap = std::unordered_map<WeakStringRef, const T, WeakStringRefHash, WeakStringRefComp>;
		template<typename T>
		using ValueNameReverseMap = std::map<T, const WeakStringRef>;

	}
}

#endif /* SF2_COMMONS_HPP_ */
