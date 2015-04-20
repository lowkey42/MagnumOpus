/***********************************************************\
 * sf2.hpp - Defines the public interface of SF2           *
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

#ifndef SF2_HPP_
#define SF2_HPP_

#include "details/commons.hpp"

#include "io/CharSource.hpp"

#include "details/ParserDefs.hpp"

template<typename T, typename M, M T::*ptr>
struct MemberParserImpl;

#include "details/sf2.hxx"


/*
 * sf2 = SFSF = Simple and Fast Struct Format
 *
 * syntax:
 *	{member: value, member2: 3, member3:[5.0, 2.1, -42.9], member4:{subMember1:a}}
 *
 *	object/map:				{a:x, b: y}
 *	vector:					[x, y]
 *
 *
 *	names/strings/enums:	[a-zA-Z_][a-zA-Z0-9_]*
 *							"[^"]*"
 *	integers:				(\+|\-)?[0-9]+
 *	floats:					(\+|\-)?[0-9]+\.[0-9]+(e(\+|\-)?[0-9]+\.[0-9]+)?
 *	chars:					.
 *	comments:				#.*
 *
 *	All unprintable or invisible characters (outside of "-strings) are ignored
 *
 *	==================
 *
 *	Examples:
 *
 *	enum class Color {
 *		RED, GREEN, BLUE
 *	};
 *	sf2_enumDef(Color,
 *			sf2_value(RED),
 *			sf2_value(ATTACK),
 *			sf2_value(BLUE)
 *	);
 *
 *	struct Position {
 *		float x, y, z;
 *	};
 *	sf2_structDef(Position,
 *		sf2_member(x),
 *		sf2_member(y),
 *		sf2_member(z)
 *	);
 *
 *	struct Player {
 *		Position position;
 *		Color color;
 *		std::string name;
 *		std::vector<Player> friends;
 *	};
 *	sf2_structDef(Player,
 *		sf2_member(position),
 *		sf2_member(color),
 *		sf2_member(name),
 *		sf2_member(friends)
 *	);
 *
 */
namespace sf2 {
	template<typename T, T v, std::size_t N>
	constexpr typename EnumDef<T>::value_type value(const char (&name)[N]) {
		return typename EnumDef<T>::value_type(name, v);
	}

	template<typename T, typename MT, MT T::*m, std::size_t N>
	constexpr typename ClassDef<T>::value_type member(const char (&name)[N]) {
		return typename ClassDef<T>::value_type( name, details::MemberParserImpl<T, MT,m>::get );
	}


	#define sf2_value(VALUE) sf2::value<sf2_cType, sf2_cType::VALUE>(#VALUE)
	#define sf2_member(MEMBER) sf2::member<sf2_cType, decltype(sf2_cType::MEMBER), &sf2_cType::MEMBER>(#MEMBER)

	#define sf2_structDef(CLASS, ...) \
		inline sf2::ClassDef<CLASS>::constructor_type _sf2_structDef(CLASS* dummy) {\
			typedef CLASS sf2_cType; \
			static const sf2::ClassDef<CLASS>::constructor_type i {__VA_ARGS__};\
			return i;\
		}

	#define sf2_enumDef(ENUMT, ...) \
		inline sf2::EnumDef<ENUMT>::constructor_type _sf2_enumDef(ENUMT dummy) {\
			typedef ENUMT sf2_cType; \
			static const sf2::EnumDef<ENUMT>::constructor_type i {__VA_ARGS__};\
			return i;\
		}


	template<typename T, std::size_t N>
	T parseString(const char (&str)[N]) {
		T obj;
		io::CStringCharSource source(str);
		ParserDefChooser<T>::get().parse(source, obj);
		return obj;
	}

	template<typename T>
	T parseString(const std::string& str) {
		T obj;
		io::StringCharSource source(str);
		ParserDefChooser<T>::get().parse(source, obj);
		return obj;
	}

	template<typename T>
	std::string writeString(const T& obj) {
		io::StrCharSink sink;
		ParserDefChooser<T>::get().write(sink, obj);
		return sink.extractString();
	}
}

#endif /* SF2_HPP_ */
