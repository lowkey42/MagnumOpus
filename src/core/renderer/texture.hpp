/**************************************************************************\
 * simple wrapper for OpenGL textures                                     *
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
#include <vector>
#include <stdexcept>
#include "../utils/log.hpp"
#include "../asset/asset_manager.hpp"

namespace mo {
namespace renderer {

	struct Texture_loading_failed : public asset::Loading_failed {
		explicit Texture_loading_failed(const std::string& msg)noexcept : Loading_failed(msg){}
	};

	class Texture {
		public:
			explicit Texture(const std::string& path) throw(Texture_loading_failed);
			explicit Texture(std::vector<uint8_t> buffer) throw(Texture_loading_failed);
			Texture(int width, int height, std::vector<uint8_t> rgba_data);
			~Texture()noexcept;

			Texture& operator=(Texture&&);

			void bind()const; // TODO: allow mutliple textures
			void unbind()const;

			auto width()const noexcept {return _width;}
			auto height()const noexcept {return _height;}

			Texture(const Texture&) = delete;
			Texture(Texture&& s) = delete;
			Texture& operator=(const Texture&) = delete;

		private:
			unsigned int _handle;
			int _width=1, _height=1;
	};
	using Texture_ptr = asset::Ptr<Texture>;

} /* namespace renderer */

namespace asset {
	template<>
	struct Loader<renderer::Texture> {
		using RT = std::shared_ptr<renderer::Texture>;

		static RT load(istream in) throw(Loading_failed){
			return std::make_shared<renderer::Texture>(in.bytes());
		}

		static void store(ostream out, renderer::Texture& asset) throw(Loading_failed) {
			// TODO
			FAIL("NOT IMPLEMENTED, YET!");
		}
	};
}
}
