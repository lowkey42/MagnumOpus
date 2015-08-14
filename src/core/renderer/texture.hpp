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
#include <glm/vec3.hpp>

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
			virtual ~Texture()noexcept;

			Texture& operator=(Texture&&)noexcept;
			Texture(Texture&& s)noexcept;

			void bind(int index=0)const;
			void unbind(int index=0)const;

			auto width()const noexcept {return _width;}
			auto height()const noexcept {return _height;}

			Texture(const Texture&) = delete;
			Texture& operator=(const Texture&) = delete;

		protected:
			Texture(int width, int height);

			unsigned int _handle;
			int _width=1, _height=1;
	};
	using Texture_ptr = asset::Ptr<Texture>;


	class Framebuffer : public Texture {
		public:
			Framebuffer(int width, int height, bool depth_buffer);
			~Framebuffer()noexcept;

			Framebuffer& operator=(Framebuffer&&)noexcept;
			Framebuffer(Framebuffer&& s)noexcept;


			void clear(glm::vec3 color=glm::vec3(0,0,0));
			void bind_target();
			void unbind_target();
			void set_viewport();

		private:
			unsigned int _fb_handle;
			unsigned int _db_handle;
	};

	struct Framebuffer_binder {
		Framebuffer_binder(Framebuffer& fb);
		~Framebuffer_binder()noexcept;

		Framebuffer& fb;
		int old_viewport[4];
	};

	struct Texture_binder {
		Texture_binder(const Framebuffer& tex, std::size_t idx=0) : tex(tex), idx(idx) {
			tex.bind(idx);
		}
		~Texture_binder()noexcept {
			tex.unbind(idx);
		}

		const Texture& tex;
		std::size_t idx;
	};


} /* namespace renderer */

namespace asset {
	template<>
	struct Loader<renderer::Texture> {
		using RT = std::shared_ptr<renderer::Texture>;

		static RT load(istream in) throw(Loading_failed){
			return std::make_shared<renderer::Texture>(in.bytes());
		}

		static void store(ostream out, const renderer::Texture& asset) throw(Loading_failed) {
			// TODO
			FAIL("NOT IMPLEMENTED, YET!");
		}
	};
}
}
