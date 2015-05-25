/**************************************************************************\
 * GUUID for all assets used in the project                               *
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
#include <memory>

namespace mo {
namespace asset {

	enum class Asset_type : uint8_t {
		gen,
		tex,
		vert_shader,
		frag_shader,
		sound,
		music,
		cfg,
		blueprint,
		font,
		anim
	};

	/**
	 * Asset_type ':' Name; not case-sensitiv; e.g. "tex:Player/main"
	 */
	class AID {
		public:
			AID() : _type(Asset_type::gen) {}
			AID(std::string n);
			AID(Asset_type c, std::string n);

			bool operator==(const AID& o)const noexcept;
			bool operator!=(const AID& o)const noexcept;
			bool operator<(const AID& o)const noexcept;
			operator bool()const noexcept;

			auto str()const noexcept -> std::string;
			auto type()const noexcept{return _type;}
			auto name()const noexcept{return _name;}

		private:
			Asset_type _type;
			std::string _name;
	};

}
}

inline mo::asset::AID operator "" _aid(const char* str, std::size_t) {
	return mo::asset::AID(str);
}

namespace std {
	template <> struct hash<mo::asset::Asset_type> {
		size_t operator()(mo::asset::Asset_type ac)const noexcept {
			return static_cast<uint8_t>(ac);
		}
	};
	template <> struct hash<mo::asset::AID> {
		size_t operator()(const mo::asset::AID& aid)const noexcept {
			auto classPart = static_cast<uint8_t>(aid.type());
			hash<string> hs;
			return classPart*size_t{71} + hs(aid.name());
		}
	};
}

