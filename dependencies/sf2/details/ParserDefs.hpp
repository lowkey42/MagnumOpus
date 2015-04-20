/***********************************************************\
 * ParserDefs.hpp - Definitions for struct & enum parsers  *
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

#ifndef SF2_PARSERDEFS_HPP_
#define SF2_PARSERDEFS_HPP_

#include "commons.hpp"
#include <cstdlib>
#include <string>
#include <type_traits>

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>

namespace sf2 {

	template<typename T>
	class ClassDef : public details::ParserFunc<T> {
		public:
			using value_type = std::pair<WeakStringRef, details::ParserFuncRef<T>>;
			using constructor_type = std::initializer_list<value_type>;

		public:
			ClassDef() : _members(_sf2_structDef(static_cast<T*>(nullptr)).begin(), _sf2_structDef(static_cast<T*>(nullptr)).end()),
				_memberOrder(_sf2_structDef(static_cast<T*>(nullptr)).begin(), _sf2_structDef(static_cast<T*>(nullptr)).end()) {}
			ClassDef(const ClassDef&)=delete;
			ClassDef& operator=(const ClassDef&)=delete;

			char parse(io::CharSource&& cs, T& obj)const {return parse(cs, obj);}
			char parse(io::CharSource& cs, T& obj)const;
			void write(io::CharSink&& sink, const T& obj)const {write(sink, obj);}
			void write(io::CharSink& sink, const T& obj)const;

		private:
			const details::MemberMap<T> _members;
			const std::vector<typename details::MemberMap<T>::value_type> _memberOrder;
	};

	template<typename T>
	class EnumDef;

	// enum-values are located through ADL of the following function based on the dummy-parameter
	// std::initializer_list<std::pair<WeakStringRef, ENUM_TYPE>> enumValues(ENUM_TYPE dummy);

	template<typename T>
	class EnumDef : public details::ParserFunc<T> {
		public:
			using value_type = std::pair<WeakStringRef, T>;
			using constructor_type = std::initializer_list<value_type>;

		public:
			EnumDef();
			EnumDef(const EnumDef&)=delete;
			EnumDef& operator=(const EnumDef&)=delete;

			char parse(io::CharSource&& cs, T& obj)const;
			char parse(io::CharSource& cs, T& obj)const;
			void write(io::CharSink& sink, const T& obj)const;

		private:
			const details::ValueNameReverseMap<T> _valuesReverse;
			const details::ValueNameMap<T> _values;
	};

	template<class T, template <typename> class TYPE>
	struct ParserDef {
		static  const details::ParserFunc<T>& get() {
			static const TYPE<T> inst;
			return inst;
		}
	};

	template<typename T>
	using ParserDefChooser = typename std::conditional<std::is_enum<T>::value, ParserDef<T, EnumDef>, ParserDef<T, ClassDef>>::type;
}

#include "ParserDefs.hxx"

#endif /* SF2_PARSERDEFS_HPP_ */
