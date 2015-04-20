/**************************************************************************\
 * logging helper                                                         *
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

#include <iostream>
#include <vector>

namespace core {
namespace util {

	struct log_target {
		virtual ~log_target()=default;

		template<typename T>
		log_target& operator<<(const T& rhs) {
			for(auto& s : _streams)
				if(*s)
					*s<<rhs;

			return *this;
		}

		log_target& operator<<(std::ostream&(*rhs)(std::ostream&)) {
			for(auto& s : _streams)
				if(*s)
					*s<<*rhs;

			if(rhs==static_cast<decltype(rhs)>(std::endl))
				on_end();

			return *this;
		}

		log_target(std::initializer_list<std::ostream*>);
		virtual void on_start(const char* level, const char* func, const char* file, int32_t line);
		virtual void on_end();

		protected:
			std::vector<std::ostream*> _streams;
	};



	extern log_target& debug(const char* func="", const char* file="", int32_t line=-1)noexcept;
	extern log_target& info (const char* func="", const char* file="", int32_t line=-1)noexcept;
	extern log_target& warn (const char* func="", const char* file="", int32_t line=-1)noexcept;
	extern log_target& error(const char* func="", const char* file="", int32_t line=-1)noexcept;
	extern log_target& fail (const char* func="", const char* file="", int32_t line=-1)noexcept;
	extern log_target& crash_report()noexcept;

}
}

#define DEBUG(M) do{::core::util::debug(__func__, __FILE__, __LINE__)<<M<<std::endl;}while(false)
#define INFO(M)  do{::core::util::info (__func__, __FILE__, __LINE__)<<M<<std::endl;}while(false)
#define WARN(M)  do{::core::util::warn (__func__, __FILE__, __LINE__)<<M<<std::endl;}while(false)
#define ERROR(M) do{::core::util::error(__func__, __FILE__, __LINE__)<<M<<std::endl;}while(false)
#define FAIL(M)  do{::core::util::fail (__func__, __FILE__, __LINE__)<<M<<std::endl; __builtin_unreachable();}while(false)
#define CRASH_REPORT(M) do{::core::util::crash_report()<<M<<std::endl;}while(false)
#define INVARIANT(C, M) do{if(__builtin_expect(!(C), false)) FAIL(M);}while(false)
