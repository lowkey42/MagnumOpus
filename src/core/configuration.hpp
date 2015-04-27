/**************************************************************************\
 * read configuration from cmd-args and config files                      *
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

#include <unordered_map>
#include <string>
#include <sstream>
#include "utils/maybe.hpp"

namespace mo {

	// use SF2 instead (cause everything is better with SF2)
	class Configuration {
		public:
			Configuration(int argc, char** argv, char** env);
			Configuration(Configuration&&) = default;
			~Configuration()noexcept = default;

			const util::maybe<const std::string&> get(const std::string& key)const {
				auto it = _properties.find(key);
				return util::justPtr(it==_properties.end() ? nullptr : &it->second);
			}

			const std::string& get(const std::string& key, const std::string& defaultVal)const {
				return get(key).get_or_other(defaultVal);
			}

			template<typename T>
			const util::maybe<const T> get_as(const std::string& key)const {
				return get(key)>> [](const std::string& val) {
					T convVal;
					std::stringstream ss;
					ss<<val;
					ss>>convVal;
					return convVal;
				};
			}

			template<typename T>
			const T get_as(const std::string& key, T default_val)const {
				return get_as<T>(key).get_or_other(default_val);
			}

			// deleted
			Configuration(const Configuration&) = delete;
			Configuration& operator=(const Configuration&) = delete;
			Configuration& operator=(Configuration&&) = default;

		private:
			std::unordered_map<std::string, std::string> _properties;
	};

} /* namespace core */
