/***********************************************************\
 * CharSource.hpp - Provides the basic API for IO          *
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

#ifndef SF2_CHARSOURCE_HPP_
#define SF2_CHARSOURCE_HPP_

#include <cstring>
#include <string>
#include <sstream>

namespace sf2 {

	struct WeakStringRef {
		const char* begin;
		const char* end;
		constexpr /*implicit*/ WeakStringRef(const char* begin, const char* end) : begin(begin), end(end){}
		template< unsigned N >
		constexpr /*implicit*/ WeakStringRef(const char (&cstring)[N]) : begin(cstring), end(cstring+N-1){}
		constexpr WeakStringRef(const WeakStringRef& o) : begin(o.begin), end(o.end){}
		WeakStringRef& operator=(const WeakStringRef& o) = delete;

		std::string toString()const {return std::string(begin, static_cast<std::size_t>(end-begin));}
	};
	struct WeakStringRefHash {
		inline std::size_t operator()(WeakStringRef r)const {
			size_t hash = 0;
			for (; r.begin!=r.end; ++r.begin)
				hash = (hash << 2) + *r.begin;
			return hash;
		}
	};
	struct WeakStringRefComp {
		inline bool operator()(WeakStringRef a, WeakStringRef b)const {
			if( a.end-a.begin != b.end-b.begin )
				return false;

			while(a.begin!=a.end) {
				if( *a.begin++ != *b.begin++ )
					return false;
			}

			return true;
		}

	};

	namespace io {
		class CharSource {
			public:
				/*implicit*/ CharSource() : _prev(0), _line(1), _column(1) {}
				virtual ~CharSource()=default;
				CharSource(const CharSource& o)=delete;
				CharSource& operator=(const CharSource& o)=delete;

				char operator()() {
					if(_lh) {
						_prev = _lh;
						_lh = 0;

					} else
						_prev = readNext();

					if( _prev=='\n' ) {
						_line++;
						_column=1;

					} else if( _prev!='\r' )
						_column++;

					return _prev;
				}
				char prev()const {return _prev;}
				std::size_t line()const {return _line;}
				std::size_t column()const {return _column;}

				char lookAhead() {
					if(_lh)
						return _lh;

					_lh = readNext();
					return _lh;
				}

			protected:
				virtual char readNext()=0;

			private:
				char _prev;
				std::size_t _line;
				std::size_t _column;
				char _lh=0;
		};
		class CStringCharSource : public CharSource {
			public:
				template< unsigned N >
				/*implicit*/ CStringCharSource(const char (&str)[N]) : _str(str) {}

			protected:
				char readNext() {
					if( _str.begin==_str.end )
						return 0;

					return *(_str.begin++);
				}

				WeakStringRef _str;
		};
		class StringCharSource : public CharSource {
			public:
				/*implicit*/ StringCharSource(const std::string& str) : _str(str), _index(0) {}

			protected:
				char readNext() {
					if( _index>=_str.length() )
						return 0;

					return _str[_index++];
				}

				const std::string& _str;
				std::size_t _index;
		};


		class CharSink {
			public:
				virtual ~CharSink()=default;

				virtual void operator()(char c)=0;

				CharSink& operator<<(const char* str ) {
					for( ; *str; ++str )
						(*this)(*str);

					return *this;
				}
				CharSink& operator<<(WeakStringRef str ) {
					for( ; str.begin!=str.end; ++str.begin )
						(*this)(*str.begin);

					return *this;
				}
			/*	template<typename T>
				CharSink& operator<<(const T& obj ) {
					std::stringstream ss;
					ss<<obj;

					(*this)<<ss.str().c_str();

					return *this;
				}*/
		};
		class StrCharSink : public CharSink {
			public:
				StrCharSink(){}
				~StrCharSink()throw(){};

				void operator()(char c) {
					_str.push_back(c);
				}

				std::string extractString() {
					return std::move(_str);
				}

			private:
				std::string _str;
		};

		inline bool skipWhitespaces( char& c, CharSource& cs ) {
			while( !isgraph(c) ) {
				c=cs();
				if(c==0)
					return false;
			}

			return true;
		}
		inline bool skipComment( char& c, CharSource& cs ) {
			if(!skipWhitespaces(c,cs))	return 0;
			if( c=='#' ) {
				while( c!='\n' ) {
					c=cs();
					if(c==0)
						return false;
				}
				while(!isgraph(c)) 	c=cs();

				if(!skipWhitespaces(c,cs))	return 0;
			}

			return true;
		}

		inline bool skipWhitespacesLH( CharSource& cs ) {
			while( !isgraph(cs.lookAhead()) || cs.lookAhead()=='\n' ) {
				if(cs()==0) // consume
					return false;
			}

			return true;
		}
		inline bool skipCommentLH( CharSource& cs ) {
			if(!skipWhitespacesLH(cs))
				return 0;

			char c = cs.lookAhead();

			if( c=='#' ) {
				cs(); // consume
				c = cs.lookAhead();

				while( c!='\n' ) {
					if(cs()==0)
						return false;

					c=cs.lookAhead();
				}

				if(!skipWhitespacesLH(cs))
					return 0;
			}

			return true;
		}

		inline bool isIdChar(char c, bool first=false) {
			return (c>='A' && c<='Z') || (c>='a' && c<='z') || (!first && c>='0' && c<='9') || c=='_';
		}

	}
}

#endif /* SF2_CHARSOURCE_HPP_ */
