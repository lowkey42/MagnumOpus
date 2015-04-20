/**************************************************************************\
 * Serializer api for component implementers                              *
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

#include "../utils/maybe.hpp"
#include "../utils/log.hpp"
#include "serializer.hpp"
#include <sf2/sf2.hpp>

namespace core {
namespace ecs {

	class Entity_state {
		public:
			Entity_state(sf2::io::CharSource& cs) : _char_source(cs) {}
			Entity_state(sf2::io::CharSink& cs) : _char_sink(cs) {}
			Entity_state(sf2::io::CharSource& source, sf2::io::CharSink& sink)
				: _char_source(source), _char_sink(sink) {}

			template<class T>
			T read_to(T&& t) {
				INVARIANT(_char_source.is_some(), "Called read_to on write-only state!");
				sf2::ParserDefChooser<T>::get().parse(_char_source.get_or_throw(), t);

				return t;
			}

			template<class T>
			void write_from(const T& source) {
				INVARIANT(_char_sink.is_some(), "Called write_from on read-only state!");
				auto& cs = _char_sink.get_or_throw();

				if(!_written) {
					cs(':');
					_written = true;
				}
				sf2::ParserDefChooser<T>::get().write(cs, source);
			}

			sf2::io::CharSource& char_source() {
				INVARIANT(_char_source.is_some(), "Called char_source on write-only state!");
				return _char_source.get_or_throw();
			}

			sf2::io::CharSink& char_sink() {
				INVARIANT(_char_sink.is_some(), "Called char_sink on write-only state!");

				auto& cs = _char_sink.get_or_throw();

				if(!_written) {
					cs(':');
					_written = true;
				}
				return cs;
			}

		private:
			util::maybe<sf2::io::CharSource&> _char_source;
			util::maybe<sf2::io::CharSink&> _char_sink;
			bool _written = false;
	};

}
}
