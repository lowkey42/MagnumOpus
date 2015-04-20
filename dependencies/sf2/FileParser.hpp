/***********************************************************\
 * FileParser.hpp - Provides a writer & parser for files   *
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

#ifndef SF2_FILEPARSER_HPP_
#define SF2_FILEPARSER_HPP_

#include "sf2.hpp"

#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

namespace sf2 {

	namespace io {
		class FileCharSource : public CharSource {
			public:
				explicit FileCharSource(std::string file) : _file(file) {}
				~FileCharSource()noexcept{}

				bool isGood()const {
					return _file.good();
				}

				bool close() {
					_file.close();
					return _file.good();
				}

			protected:
				char readNext() {
					if( _file.eof() || _file.bad() )
						return 0;

					return _file.get();
				}

				std::ifstream _file;
		};

		class FileCharSink : public CharSink {
			public:
				explicit FileCharSink(std::string file) : _file(file) {}
				~FileCharSink()noexcept {}

				bool close() {
					_file.close();
					return _file.good();
				}

				bool isGood()const {
					return _file.good();
				}

				void operator()(char c) {
					_file.put(c);
				}

			private:
				std::ofstream _file;
		};
	}  // namespace io

	template<typename T>
	bool parseFile(std::string filePath, T& target) {
		io::FileCharSource source(filePath);
		if( !source.isGood() )
			return false;

		ParserDefChooser<T>::get().parse(source, target);

		return source.close();
	}

	template<typename T>
	bool writeFile(std::string filePath, const T& obj) {
		io::FileCharSink sink(filePath);
		if( !sink.isGood() )
			return false;

		ParserDefChooser<T>::get().write(sink, obj);
		return sink.close();
	}

}

#endif /* SF2_FILEPARSER_HPP_ */
