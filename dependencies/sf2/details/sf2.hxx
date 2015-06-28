/***********************************************************\
 * ParserDefs.hxx - Parsers for buildin & STL types        *
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

#ifndef SF2_HXX_
#define SF2_HXX_

#include "commons.hpp"
#include "ParserDefs.hpp"
#include <iostream>
#include <cmath>
#include <cstdio>

namespace sf2 {
	namespace details {

		namespace helpers {
			inline char parseNumberPreprocessing(io::CharSource& cs, bool& negativ) {
				char c=cs();

				if(!skipWhitespaces(c,cs))	return 0;

				if( c=='-' || c=='+' ) {
					negativ = c=='-';
					c=cs();
				}

				if(!skipWhitespaces(c,cs))	return 0;

				return c;
			}
			template<typename T>
			char parseIntegerPart(char c, io::CharSource& cs, T& val) {
				for(; c>='0' && c<='9'; c=cs())
					val= (10*val)+ (c-'0');

				return c;
			}
			template<typename T>
			char parseDecimalPart(io::CharSource& cs, T& val) {
				char c=cs();
				for(double dec=10; c>='0' && c<='9'; c=cs(), dec*=10.f)
					val+= (c-'0') / dec;

				return c;
			}

			template<typename T>
			char parseInteger(io::CharSource& cs, T& val) {
				bool negativ=false;
				char c=parseNumberPreprocessing(cs, negativ);

				val = 0;
				c= parseIntegerPart(c, cs, val);

				if( negativ )
					val*=-1;

				return c;
			}
			constexpr uint64_t maxDec(std::size_t byteSize) {
				return byteSize==1 ? 100ul /*10^3*/ :
					(byteSize==2 ? 100000ul /*10^5*/ :
						( byteSize==4 ? 10000000000ul /*10^10*/ : 10000000000000000000ul /*10^19*/ )
					);
			}
			template<typename T>
			void writeInteger(io::CharSink& sink, T val) {
				static_assert(sizeof(T)<=8, "integer >uint64_t is unsupported!");

				if(val<0)
					sink('-');

				if( val==0 ) {
					sink('0');
					return;
				}

				bool first = true;
				val = val>=0?val:-val;
				for(auto p=maxDec(sizeof(T)); p>0; p/=10) {
					char d = ((val/p)%10) + '0';
					if( d!='0' || !first ) {
						first = false;
						sink(d);
					}
				}
			}

			template<typename T>
			char parseFloat(io::CharSource& cs, T& val) {
				bool negativ=false;
				char c=parseNumberPreprocessing(cs, negativ);

				val = 0;
				c= parseIntegerPart(c, cs, val);

				if( c=='.' ) {
					c= parseDecimalPart(cs, val);
				}

				if( c=='e' || c=='E' ) {
					int exp;
					c = parseInteger(cs, exp);
					val *= (T) std::pow(exp>0 ? 10.0 : 0.1, static_cast<double>(exp>=0?exp:-exp));
				}

				if( negativ )
					val*=-1;

				return c;
			}
			template<typename T>
			void writeFloat(io::CharSink& sink, T val) {
				char buffer[64];

				int l = std::min( snprintf(buffer, 64, "%g", val), 64);

				if(l<=0) {
					std::cerr<<"Unable to write float: "<<static_cast<double>(val)<<std::endl;
					sink('0');
					return;
				}

				for(int i=0; i<l; i++ )
					sink(buffer[i]);
			}

		}
		
		
		namespace fundamentals {
			template<typename M>
			char _parseMember(io::CharSource& cs, M& val);

			template<typename M>
			void _writeMember(io::CharSink& sink, const M& val);


			template<> inline char _parseMember(io::CharSource& cs, char& val) {
				char c=cs();

				if(!skipWhitespaces(c,cs))	return 0;

				if( c=='"' || c=='\'' ) {
					val = cs();
					c=cs();
					if( c!='"' && c!='\'' )
						return onError("invalid character (string length!=1)", cs) ? c : 0;

					return cs();

				} else {
					val = c;
					return cs();
				}
			}
			template<> inline void _writeMember(io::CharSink& sink, const char& val) {
				sink('"');
				sink(val);
				sink('"');
			}


			template<> inline char _parseMember(io::CharSource& cs, uint8_t& val) 	{	return helpers::parseInteger(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, int16_t& val) 	{	return helpers::parseInteger(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, uint16_t& val) {	return helpers::parseInteger(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, int32_t& val) 	{	return helpers::parseInteger(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, uint32_t& val) {	return helpers::parseInteger(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, int64_t& val) 	{	return helpers::parseInteger(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, uint64_t& val) {	return helpers::parseInteger(cs, val);	}

			template<> inline void _writeMember(io::CharSink& sink, const uint8_t& val) 	{	helpers::writeInteger(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const int16_t& val) 	{	helpers::writeInteger(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const uint16_t& val) 	{	helpers::writeInteger(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const int32_t& val) 	{	helpers::writeInteger(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const uint32_t& val) 	{	helpers::writeInteger(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const int64_t& val) 	{	helpers::writeInteger(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const uint64_t& val) 	{	helpers::writeInteger(sink, val);	}


			template<> inline char _parseMember(io::CharSource& cs, float& val) 	{	return helpers::parseFloat(cs, val);	}
			template<> inline char _parseMember(io::CharSource& cs, double& val) 	{	return helpers::parseFloat(cs, val);	}

			template<> inline void _writeMember(io::CharSink& sink, const float& val) 	{	helpers::writeFloat(sink, val);	}
			template<> inline void _writeMember(io::CharSink& sink, const double& val) {	helpers::writeFloat(sink, val);	}


			template<> inline char _parseMember(io::CharSource& cs, bool& val) 	{
				char c=cs();

				if(!skipWhitespaces(c,cs))	return 0;

				auto matchOrFail = [&](const char* str) -> char {
					for (const char *it = str; *it; ++it, c=cs())
						if (*it != c)
							return onError(std::string("invalid character for boolean: ")+c, cs) ? c : 0;

					return cs.prev();
				};


				if( c=='f' ) {
					if( matchOrFail("false")==0 ) return 0;
					val = false;
					return c;

				} else if( c=='t' ) {
					if( matchOrFail("true")==0 ) return 0;
					val = true;
					return c;

				} else if( c=='o' ) {
					if( (c=cs())=='n' ) {
						val = true;
						return c;

					} else if(c=='f') {
						if( matchOrFail("ff")==0 ) return 0;
						val = false;
						return c;
					}
				}

				return onError(std::string("invalid character for boolean: ")+c, cs) ? c : 0;
			}
			template<> inline void _writeMember(io::CharSink& sink, const bool& val) {
				sink<< (val ? "true" : "false");
			}


			template<> inline char _parseMember(io::CharSource& cs, std::string& val) {
				char c=cs();

				if(!skipWhitespaces(c,cs))	return 0;

				val.clear();
				val.reserve(std::max(val.capacity(), static_cast<std::size_t>(64)));

				if( c=='"' || c=='\'' ) { 	//< quoted-string
					char eosm = c;
					for(c=cs(); c!=eosm; c=cs())
						val.push_back(c);

					c=cs();

				} else {		//< unquoted-string
					bool first=true;
					while( io::isIdChar(c, first) ) {
						val.push_back(c);
						c=cs();
						first = false;
					}
				}

				return c;
			}
			template<> inline void _writeMember(io::CharSink& sink, const std::string& val) {
				sink('"');

				for( const char c : val )
					sink(c);

				sink('"');
			}
		}


		enum class MemberType {
			FUNDAMENTAL,
			ENUM,
			COMPLEX,
		};

		template<typename T, MemberType memberType>
		struct MemberParser;

		template<typename T>
		using MemberParserChooser = MemberParser<T,
				std::conditional<std::is_fundamental<T>::value,
				typename std::integral_constant<MemberType, MemberType::FUNDAMENTAL>,
				typename std::conditional<std::is_enum<T>::value,
						std::integral_constant<MemberType, MemberType::ENUM>,
						std::integral_constant<MemberType, MemberType::COMPLEX> >::type
				>::type::value >;

		template<typename T, MemberType memberType>
		struct MemberParser : public details::ParserFunc<T> {
			static const details::ParserFunc<T>& get() {
				static const MemberParser<T, memberType> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, T& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const T& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, T& obj) {
				return ParserDefChooser<T>::get().parse(cs, obj);
			}
			static void _write(io::CharSink& sink, const T& obj) {
				ParserDefChooser<T>::get().write(sink, obj);
			}
		};

		/**
		 * Overload for fundamental types
		 * uses _parseMember(...) and _writeMember(...)
		 */
		template<typename T>
		struct MemberParser<T, MemberType::FUNDAMENTAL> : public ParserFunc<T> {
			static const ParserFunc<T>& get() {
				static const MemberParser<T, MemberType::FUNDAMENTAL> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, T& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const T& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, T& obj) {
				return fundamentals::_parseMember(cs, obj);
			}
			static void _write(io::CharSink& sink, const T& obj) {
				fundamentals::_writeMember(sink, obj);
			}
		};

		/**
		 * Overload for strings
		 */
		template<>
		struct MemberParser<std::string, MemberType::COMPLEX> : public details::ParserFunc<std::string> {
			static const details::ParserFunc<std::string>& get() {
				static const MemberParser<std::string, MemberType::COMPLEX> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, std::string& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const std::string& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, std::string& obj) {
				return fundamentals::_parseMember(cs, obj);
			}
			static void _write(io::CharSink& sink, const std::string& obj) {
				fundamentals::_writeMember(sink, obj);
			}
		};

		template<class SubT>
		struct MemberParser<std::unique_ptr<SubT>, MemberType::COMPLEX> : public ParserFunc<std::unique_ptr<SubT>> {
			static const  ParserFunc<std::unique_ptr<SubT>>& get() {
				static const MemberParser<std::unique_ptr<SubT>, MemberType::COMPLEX> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, std::unique_ptr<SubT>& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const std::unique_ptr<SubT>& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, std::unique_ptr<SubT>& obj) {
				obj.reset(new SubT);
				return MemberParserChooser<SubT>::_parse(cs, *(obj.get()));
			}
			static void _write(io::CharSink& sink, const std::unique_ptr<SubT>& obj) {
				if( obj )
					MemberParserChooser<SubT>::_write(sink, *(obj.get()));
			}
		};
		template<class SubT>
		struct MemberParser<std::shared_ptr<SubT>, MemberType::COMPLEX> : public ParserFunc<std::shared_ptr<SubT>> {
			static const  ParserFunc<std::shared_ptr<SubT>>& get() {
				static const MemberParser<std::shared_ptr<SubT>, MemberType::COMPLEX> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, std::shared_ptr<SubT>& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const std::shared_ptr<SubT>& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, std::shared_ptr<SubT>& obj) {
				obj = std::make_shared<SubT>();
				return MemberParserChooser<SubT>::_parse(cs, *(obj.get()));
			}
			static void _write(io::CharSink& sink, const std::shared_ptr<SubT>& obj) {
				if( obj )
					MemberParserChooser<SubT>::_write(sink, *(obj.get()));
			}
		};

		template<class SubT>
		struct MemberParser<std::vector<SubT>, MemberType::COMPLEX> : public ParserFunc<std::vector<SubT>> {
			static const  ParserFunc<std::vector<SubT>>& get() {
				static const MemberParser<std::vector<SubT>, MemberType::COMPLEX> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, std::vector<SubT>& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const std::vector<SubT>& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, std::vector<SubT>& obj) {
				char c=cs();

				if(!skipComment(c,cs))	return 0;

				if( c!='[' )
					return onError(std::string("list definition has to start with '[', found:")+c, cs) ? c : 0;

				while( c!=']' ) {
					skipCommentLH(cs);
					if(cs.lookAhead()==']') {
						cs();
						break;
					}

					SubT elem;
					if( !(c=MemberParserChooser<SubT>::_parse(cs, elem)) )
						return 0;

					obj.push_back(std::move(elem));

					if(!skipComment(c,cs))	return 0;
				}

				return cs();
			}
			static void _write(io::CharSink& sink, const std::vector<SubT>& obj) {
				sink<<"[\n";

				bool first = true;
				for( const SubT& e : obj ) {
					if( first )
						first = false;
					else
						sink(',');

					MemberParserChooser<SubT>::_write(sink, e);
				}

				sink<<"\n]";
			}
		};

		template<class SubT_key, class SubT_value>
		struct MemberParser<std::map<SubT_key, SubT_value>, MemberType::COMPLEX> : public ParserFunc<std::map<SubT_key, SubT_value>> {
			static const  ParserFunc<std::map<SubT_key, SubT_value>>& get() {
				static const MemberParser<std::map<SubT_key, SubT_value>, MemberType::COMPLEX> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, std::map<SubT_key, SubT_value>& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const std::map<SubT_key, SubT_value>& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, std::map<SubT_key, SubT_value>& obj) {
				char c=cs();

				if(!skipComment(c,cs))	return 0;

				if( c!='{' )
					return onError(std::string("map definition has to start with '{', found: ")+c, cs) ? c : 0;

				while( c!='}' ) {
					SubT_key key;

					if( !(c=MemberParserChooser<SubT_key>::_parse(cs, key)) )
						return 0;

					if(!skipComment(c,cs))	return 0;

					if( c!=':' )
						return onError(std::string("missing ':' in map, found: ")+c, cs) ? c : 0;

					SubT_value val;
					if( !(c=MemberParserChooser<SubT_value>::_parse(cs, val)) )
						return 0;

					obj.insert(std::make_pair(std::move(key), std::move(val)));

					if(!skipComment(c,cs))	return 0;
				}

				return cs();
			}
			static void _write(io::CharSink& sink, const std::map<SubT_key, SubT_value>& obj) {
				sink<<"{\n";

				bool first = true;
				for( const auto& i : obj ) {
					if( first )
						first = false;
					else
						sink(',');

					MemberParserChooser<SubT_key>::_write(sink, i.first);
					sink(':');
					MemberParserChooser<SubT_value>::_write(sink, i.second);
				}

				sink<<"\n}";
			}
		};

		template<class SubT_key, class SubT_value>
		struct MemberParser<std::unordered_map<SubT_key, SubT_value>, MemberType::COMPLEX> : public ParserFunc<std::unordered_map<SubT_key, SubT_value>> {
			static const  ParserFunc<std::unordered_map<SubT_key, SubT_value>>& get() {
				static const MemberParser<std::unordered_map<SubT_key, SubT_value>, MemberType::COMPLEX> inst;
				return inst;
			}

			MemberParser()noexcept{};

			char parse(io::CharSource& cs, std::unordered_map<SubT_key, SubT_value>& obj)const {
				return _parse(cs, obj);
			}
			void write(io::CharSink& sink, const std::unordered_map<SubT_key, SubT_value>& obj)const {
				_write(sink, obj);
			}

			static char _parse(io::CharSource& cs, std::unordered_map<SubT_key, SubT_value>& obj) {
				char c=cs();

				if(!skipComment(c,cs))	return 0;

				if( c!='{' )
					return onError(std::string("map definition has to start with '{', found: ")+c, cs) ? c : 0;

				while( c!='}' ) {
					SubT_key key;

					if( !(c=MemberParserChooser<SubT_key>::_parse(cs, key)) )
						return 0;

					if(!skipComment(c,cs))	return 0;

					if( c!=':' )
						return onError(std::string("missing ':' in map, found: ")+c, cs) ? c : 0;

					SubT_value val;
					if( !(c=MemberParserChooser<SubT_value>::_parse(cs, val)) )
						return 0;

					obj.insert(std::make_pair(std::move(key), std::move(val)));

					if(!skipComment(c,cs))	return 0;
				}

				return cs();
			}
			static void _write(io::CharSink& sink, const std::unordered_map<SubT_key, SubT_value>& obj) {
				sink<<"{\n";

				bool first = true;
				for( const auto& i : obj ) {
					if( first )
						first = false;
					else
						sink(',');

					MemberParserChooser<SubT_key>::_write(sink, i.first);
					sink(':');
					MemberParserChooser<SubT_value>::_write(sink, i.second);
				}

				sink<<"\n}";
			}
		};


		template<typename T, typename M, M T::*ptr>
		struct MemberParserImpl : public ParserFunc<T> {
			MemberParserImpl()noexcept{};

			static const  ParserFunc<T>& get() {
				static const MemberParserImpl<T,M,ptr> inst;
				return inst;
			}

			char parse(io::CharSource& cs, T& obj)const {
				return MemberParserChooser<M>::_parse(cs, obj.*ptr);
			}
			void write(io::CharSink& sink, const T& obj)const {
				MemberParserChooser<M>::_write(sink, obj.*ptr);
			}
		};

	} /* namespace details */
} /* namespace sf2 */

#endif /* SF2_HXX */
