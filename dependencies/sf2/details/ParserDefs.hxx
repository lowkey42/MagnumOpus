/***********************************************************\
 * ParserDefs.hxx - Implementation of struct & enum parsers*
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

#ifndef SF2_PARSERDEFS_HXX_
#define SF2_PARSERDEFS_HXX_

namespace sf2 {
	namespace details {
		template<std::size_t N>
		WeakStringRef readIdentifier(io::CharSource& source, char (&buffer)[N]) {
			char c=source();
			if(!skipComment(c,source)) {
				onError("eof reading identifier", source);
				return WeakStringRef(buffer,buffer);
			}

			if(c=='"' || c=='\'')
				c=source();

			bool first=true;
			for(std::size_t i=0; i<N; ++i ) {
				if( io::isIdChar(c, first) ) {
					buffer[i]=c;
					c=source();
					first = false;

				} else {
					if(c=='"' || c=='\'')
						c=source();

					buffer[i]='\0';

					return WeakStringRef(buffer, buffer+i);
				}
			}

			return WeakStringRef(buffer, buffer+N-1);
		}

		template<typename T, typename iterator>
		ValueNameReverseMap<T> buildReverseMap(iterator begin, iterator end) {
			ValueNameReverseMap<T> r;
			for( ; begin!=end; begin++ )
				r.insert(std::make_pair(begin->second, begin->first));

			return r;
		}
	}


	/// IMPL ClassDef:
	template<typename T>
	char ClassDef<T>::parse(io::CharSource& cs, T& obj)const {
		char c=cs();

		if(!skipComment(c,cs))	return 0;

		if( c!='{' )
			return onError(std::string("object definition has to start with '{', found: ")+c, cs) ? c : 0;

		char memberBuffer[details::MAX_MEMBER_NAME_LENGTH];

		while( c!='}' ) {
			WeakStringRef id = details::readIdentifier(cs, memberBuffer);
			c=cs.prev();

			if( id.begin==id.end )
				return onError(std::string("invalid identifier in object '")+id.toString()+"', next char: "+c, cs) ? c : 0;

			if(!skipComment(c,cs))	return 0;

			if( c!=':' )
				return onError(std::string("missing ':' after member name '")+id.toString()+"', found: "+c, cs) ? c : 0;

			auto mp = _members.find(id);
			if( mp==_members.end() )
				return onError(std::string("unknown member-name '")+id.toString()+"', nextChar: "+c, cs) ? c : 0;

			if( !(c=mp->second().parse(cs, obj)) )
				return 0;

			if(!skipComment(c,cs))	return 0;
		}

		return cs();
	}
	template<typename T>
	void ClassDef<T>::write(io::CharSink& sink, const T& obj)const {
		sink<<"{\n";

		bool first = true;
		for( auto& m : _memberOrder ) {
			if( first )
				first = false;
			else
				sink(',');

			sink<< m.first << ": ";

			m.second().write( sink, obj );
		}

		sink<<"\n}";
	}


	// IMPL EnumDef:
	template<typename T>
	EnumDef<T>::EnumDef()
		: _valuesReverse(details::buildReverseMap<T>(_sf2_enumDef(static_cast<T>(0)).begin(), _sf2_enumDef(static_cast<T>(0)).end())),
		  _values(_sf2_enumDef(static_cast<T>(0)).begin(), _sf2_enumDef(static_cast<T>(0)).end()) {}

	template<typename T>
	char EnumDef<T>::parse(io::CharSource&& cs, T& obj)const {
		return parse(cs, obj);
	}
	template<typename T>
	char EnumDef<T>::parse(io::CharSource& cs, T& obj)const {
		char valBuffer[details::MAX_ENUM_VALUE_LENGTH];

		WeakStringRef id = details::readIdentifier(cs, valBuffer);

		auto v = _values.find(id);
		if( v==_values.end() )
			return onError(std::string("unknown enum-value '")+id.toString()+"'", cs) ? cs.prev() : 0;

		obj = v->second;
		return cs.prev();
	}
	template<typename T>
	void EnumDef<T>::write(io::CharSink& sink, const T& obj)const {
		auto n = _valuesReverse.find(obj);
		if( n!=_valuesReverse.end() ) {
			sink<< n->second;
		}
	}
}

#endif /* SF2_PARSERDEFS_HXX_ */
