#include "text.hpp"


namespace mo {
namespace renderer {

	Vertex_layout text_vertex_layout{
		Vertex_layout::Mode::triangles,
		vertex("position", &Font_vertex::xy),
		vertex("uv", &Font_vertex::uv)
	};


	Font::Font(asset::Asset_manager& assets, std::istream& stream) {
		std::string line;
		std::getline(stream, line);

		int size=0;
		stream>>size;
		stream>>_height;
		_line_height = _height;

		std::getline(stream, line);
		std::getline(stream, line);
		util::replace_inplace(line, "\r", "");
		_texture = assets.load<Texture>(asset::AID(asset::Asset_type::tex, line));

		int symbols;
		stream>>symbols;

		_glyphs.reserve(symbols);
		for(auto i : util::range(symbols)) {
			Text_char id = i;
			stream>>id;

			Glyph& g = _glyphs[id];
			stream>> g.x >> g.y >> g.width >> g.height >> g.offset_x >> g.offset_y >> g.advance;
		}

		if(stream.good()) {
			int kernings = 0;
			stream>> kernings;

			for(auto i : util::range(kernings)) {
				Text_char id=i, prev;
				int val=0;
				stream>> id >> prev >> val;

				_glyphs[id].kerning[prev] = val;
			}
		}
	}

	using glm::vec2;

	namespace {
		void create_quad(std::vector<Font_vertex>& vertices,
		                float x, float y,
		                float u, float v,
		                float w, float h,
		                float tw,float th) {
			vertices.push_back({{x  ,y  },   {u    /tw, 1-(v  )/th}});
			vertices.push_back({{x  ,(y+h)}, {u    /tw, 1-(v+h)/th}});
			vertices.push_back({{x+w,y  },   {(u+w)/tw, 1-(v  )/th}});

			vertices.push_back({{x+w,(y+h)}, {(u+w)/tw, 1-(v+h)/th}});
			vertices.push_back({{x+w,y  },   {(u+w)/tw, 1-(v  )/th}});
			vertices.push_back({{x  ,(y+h)}, {u    /tw, 1-(v+h)/th}});
		}

		template<typename Func>
		void parse(const std::string str, int height, int tex_width, int tex_height,
		           const std::unordered_map<Text_char, Glyph>& glyphs, Func quad_callback) {
			glm::vec2 offset{0,-height};
			Text_char prev = 0;

			auto tw = tex_width;
			auto th = tex_height;

			auto add_glyph = [&](Text_char c) {
				if(c=='\n') {
					offset.x=0;
					offset.y+=height;
					return;
				}

				auto g = glyphs.find(c);
				if(g==glyphs.end())
					g=glyphs.begin();

				auto& glyph = g->second;

				auto k = glyph.kerning.find(prev);
				if(k!=glyph.kerning.end())
					offset.x+=k->second;

				quad_callback(
				            offset.x + glyph.offset_x,
				            offset.y + glyph.offset_y,
				            glyph.x,
				            glyph.y,
				            glyph.width,
				            glyph.height,
				            tw, th);


				offset.x+= glyph.advance;

				prev = c;
			};

			for(auto iter=str.begin(); iter!=str.end(); ++iter) {
				auto b0 = *iter >> 7;

				if(b0==0)
					add_glyph(*iter);

				else {
					auto b1 = *iter >>6 & 1;
					auto b2 = *iter >>5 & 1;
					auto b3 = *iter >>4 & 1;

					if(b1==0) { // 1 byte
						add_glyph(*iter);

					} else if(b2==0) { // 2 byte
						auto byte_1 = (*iter & 0b00011111);
						iter++;
						auto byte_2 = (*iter & 0b00111111);
						add_glyph(byte_1<<6 | byte_2);

					} else if(b3==0) { // 3 byte
						auto byte_1 = (*iter & 0b00011111);
						iter++;
						auto byte_2 = (*iter & 0b00111111);
						iter++;
						auto byte_3 = (*iter & 0b00111111);
						add_glyph((byte_1<<6 | byte_2)<<6 | byte_3);

					} else { // 4 byte
						auto byte_1 = (*iter & 0b00011111);
						iter++;
						auto byte_2 = (*iter & 0b00111111);
						iter++;
						auto byte_3 = (*iter & 0b00111111);
						iter++;
						auto byte_4 = (*iter & 0b00111111);
						add_glyph(((byte_1<<6 | byte_2)<<6 | byte_3)<<6 | byte_4);
					}
				}

			}
		}
	}

	void Font::calculate_vertices(const std::string& str,
	                              std::vector<Font_vertex>& vertices)const {

		vertices.reserve(str.length()*4);

		parse(str, _height, _texture->width(), _texture->height(), _glyphs, [&](auto... args) {
			create_quad(vertices, args...);
		});
	}

	auto Font::text(const std::string& str)const -> Text_ptr {
		auto& entry =_cache[str];

		if(entry)
			return entry;

		std::vector<Font_vertex> vertices;
		calculate_vertices(str, vertices);

		return entry=std::make_shared<Text>(vertices);
	}

	auto Font::calculate_size(const std::string& str)const -> glm::vec2 {
		glm::vec2 top_left, bottom_right;

		parse(str, _height, _texture->width(), _texture->height(), _glyphs, [&](
		      float x, float y,
		      float u, float v,
              float w, float h,
              float tw,float th) {
			if(x<top_left.x) top_left.x=x;
			if(y<top_left.y) top_left.y=y;

			if(x+w>bottom_right.x) bottom_right.x=x;
			if(y+h>bottom_right.y) bottom_right.y=y;
		});

		return bottom_right - top_left;
	}

	void Font::bind()const{
		_texture->bind();
	}


	Text::Text(std::vector<Font_vertex> vertices)
	    : _obj(text_vertex_layout, create_buffer(vertices)) {
	}
	void Text::draw()const {
		_obj.draw();
	}

	Text_dynamic::Text_dynamic(Font_ptr font)
	    : _font(font),
	      _data(),
	      _obj(text_vertex_layout, create_dynamic_buffer<Font_vertex>(4*5)) {
	}

	void Text_dynamic::draw()const {
		_obj.draw();
	}
	void Text_dynamic::set(const std::string& str) {
		_data.clear();
		_font->calculate_vertices(str, _data);
		_obj.buffer().set(_data);
	}

}
}
