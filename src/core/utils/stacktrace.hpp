/**************************************************************************\
 * stacktrace generator & error handler                                   *
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

#include <string>
#include <stdexcept>

namespace core {
namespace util {

	extern void init_stacktrace(std::string exe_path);

	extern bool is_stacktrace_available();

	extern std::string gen_stacktrace(int frames_to_skip=0);

	struct Error : public std::runtime_error {
		explicit Error(const std::string& msg)
		  : std::runtime_error(msg+"\n At "+gen_stacktrace(1)) {}
	};

}
}

