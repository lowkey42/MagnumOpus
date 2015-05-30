/**************************************************************************\
 * iostreams for assets                                                   *
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

#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include "../utils/template_utils.hpp"

#include "../utils/stacktrace.hpp"

#include "aid.hpp"


namespace mo {
namespace asset {
	struct File_handle;
	class Asset_manager;

	struct Loading_failed : public util::Error {
		explicit Loading_failed(const std::string& msg)noexcept : util::Error(msg) {}
	};

	class stream {
		public:
			stream(AID aid, Asset_manager& manager, File_handle* file, const std::string& path);
			stream(stream&&);
			stream(const stream&)=delete;
			~stream()noexcept;

			stream& operator=(const stream&) = delete;
			stream& operator=(stream&&) = default;

			auto eof()const noexcept -> bool;
			auto length()const noexcept -> size_t;

			auto aid()const noexcept {return _aid;}
			auto& manager()noexcept {return _manager;}

		protected:
			File_handle* _file;
			AID _aid;
			Asset_manager& _manager;

			class fbuf;
			std::unique_ptr<fbuf> _fbuf;
	};
	class istream : public stream, public std::istream {
		public:
			istream(AID aid, Asset_manager& manager, const std::string& path);
			istream(istream&&);

			istream& operator=(istream&&) = default;

			auto lines() -> std::vector<std::string>;
			auto content() -> std::string;
			auto bytes() -> std::vector<uint8_t>;
	};
	class ostream : public stream, public std::ostream {
		public:
			ostream(AID aid, Asset_manager& manager, const std::string& path);
			ostream(ostream&&);

			ostream& operator=(ostream&&) = default;
	};

	/**
	 * Specialize this template for each asset-type
	 * Instances should be lightweight
	 * Implementations should NEVER return nullptr
	 */
	template<class T>
	struct Loader {
		static_assert(util::dependent_false<T>(), "Required AssetLoader specialization not provided.");

		static auto load(istream in) throw(Loading_failed) -> std::shared_ptr<T>;
		static void store(ostream out, const T& asset) throw(Loading_failed);
	};

}
}
