/**************************************************************************\
 * simple text renderer (bitmap fonts)                                    *
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

#include "../asset/asset_manager.hpp"
#include "texture.hpp"
#include "vertex_object.hpp"

namespace mo {
namespace renderer {

	class Text;
	using Text_ptr = std::shared_ptr<const Text>;

	using Text_char = uint32_t;

	struct Font_vertex {
		glm::vec2 xy;
		glm::vec2 uv;
		Font_vertex(glm::vec2 xy, glm::vec2 uv) : xy(xy), uv(uv) {}
	};
	extern Vertex_layout text_vertex_layout;

	struct Glyph {
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		int offset_x = 0;
		int offset_y = 0;
		int advance = 0;
		std::unordered_map<Text_char, int> kerning;
	};

	/*
	 * Format:
	 * family
	 * size height
	 * texture
	 * symbol_count
	 * id x y width height offset_x offset_y advance
	 * ...
	 * kerning_count
	 * id prev_char value
	 * ...
	 */
	class Font {
		public:
			Font(asset::Asset_manager& assets, std::istream&);

			auto text(const std::string str)const -> Text_ptr;
			auto calculate_size(const std::string str)const -> glm::vec2;
			void bind()const;

		private:
			friend class Text;

			int _height = 0;
			int _line_height = 0;
			Texture_ptr _texture;
			std::unordered_map<Text_char, Glyph> _glyphs;

			mutable std::unordered_map<std::string, Text_ptr> _cache;
	};
	using Font_ptr = asset::Ptr<Font>;

	class Text {
		public:
			Text(std::vector<Font_vertex> vertices);

			void draw()const;

		private:
			Object _obj;
	};

}

namespace asset {
	template<>
	struct Loader<renderer::Font> {
		using RT = std::shared_ptr<renderer::Font>;

		static RT load(istream in) throw(Loading_failed){
			return std::make_shared<renderer::Font>(in.manager(), in);
		}

		static void store(ostream out, renderer::Texture& asset) throw(Loading_failed) {
			// TODO
			FAIL("NOT IMPLEMENTED, YET!");
		}
	};
}
}
