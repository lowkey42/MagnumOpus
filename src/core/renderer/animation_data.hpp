/**************************************************************************\
 * animation_data -													      *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <unordered_map>

#include "../renderer/texture.hpp"
#include "../../core/asset/asset_manager.hpp"

namespace mo {
namespace renderer {

	enum class Animation_type{
		idle,
		moving,
		attack
	};
}
}

namespace std {
	template <> struct hash<mo::renderer::Animation_type> {
		size_t operator()(mo::renderer::Animation_type ac)const noexcept {
			return static_cast<size_t>(ac);
		}
	};
}

namespace mo {
namespace renderer {

	struct Animation_frame_data{
		int row;
		float fps;
		int frames;
	};

	struct Animation{
		int frame_width;
		int frame_height;
		Texture_ptr texture;
		std::string texName = texture.aid().name();
		std::unordered_map<Animation_type, Animation_frame_data> animations;
	};

}

namespace asset {
	template<>
	struct Loader<renderer::Animation> {
		using RT = std::shared_ptr<renderer::Animation>;

		static RT load(istream in) throw(Loading_failed);

		static void store(ostream out, renderer::Animation& asset) throw(Loading_failed);
	};
}
}

