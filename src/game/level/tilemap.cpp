#define GLM_SWIZZLE

#include "tilemap.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <math.h>

#include "level.hpp"

namespace mo {
namespace level {

	using namespace renderer;

	bool talkative = false;

	// layout description for vertices
	Vertex_layout layout {
		Vertex_layout::Mode::triangles,
		vertex("position",	&Tilemap::TileVertex::pos),
		vertex("uv",		&Tilemap::TileVertex::uv),
		vertex("layer",		&Tilemap::TileVertex::layer)
	};

	Tilemap::Tilemap(Engine &engine, const Level &lev)
	    : _level(lev),
		  _object(layout, create_dynamic_buffer<TileVertex>(16*16*6)) {
		// Create and attach the Shader
		_shader.attach_shader(engine.assets().load<Shader>("vert_shader:tilemap"_aid))
				.attach_shader(engine.assets().load<Shader>("frag_shader:tilemap"_aid))
		        .bind_all_attribute_locations(layout)
		        .build();

		// Load a predefined texture and bind it
		_texture = engine.assets().load<Texture>("tex:tilemap"_aid);

		_tex_width = _texture->width();
		_tex_height = _texture->height();

		// TODO: [Sebastian]
		// Accessing camera.world_scale() and appending it to _tile_tex_heigth & _tile_tex_width
		// for further use in calculations

		_tpl = _tex_width / _tile_tex_width;

		_xTexTile = _tile_tex_width / static_cast<double>(_tex_width);
		_yTexTile = _tile_tex_height / static_cast<double>(_tex_height);
	}


	void Tilemap::draw(const Camera& cam){
		auto cam_area  = cam.area();
		auto cam_start = glm::vec2{cam_area.x, cam_area.y};
		auto cam_end   = glm::vec2{cam_area.z, cam_area.w};

		auto cam_range = glm::abs(cam_end-cam_start);
		_vertices.clear();
		_vertices.reserve(std::ceil(cam_range.x * cam_range.y) * 6);

		// Filling the vector with vertices of the displayed triangles and corresponding uv-data
		_level.foreach_tile(cam_start.x, cam_start.y, cam_end.x, cam_end.y,
		                    [&](int x, int y, const auto& tile) {

			int tile_type = static_cast<int>(tile.type);
			// factor for horizontal selection of texture-part
			int tile_x = tile_type % _tpl;
			// factor for vertical selection of texture-part
			// if x > maximum tile per line --> do a wrap to the next line (y)
			int tile_y = std::floor(tile_type / _tpl) + 1;

			float layer = 0;
			if(tile_type == 0 || (tile_type >= 4 && tile_type <= 6))
				layer = 0.99;

			// First triangle of the rectangle
			_vertices.push_back({{ x - 0.5f, y - 0.5f }, {tile_x*(_xTexTile), 1.0-(tile_y*_yTexTile)}, layer});
			_vertices.push_back({{ x - 0.5f, y + 0.5f }, {tile_x*(_xTexTile), 1.0-((tile_y - 1) *_yTexTile)} , layer});
			_vertices.push_back({{ x + 0.5f, y + 0.5f }, {(tile_x+1)*(_xTexTile), 1.0-((tile_y - 1) *_yTexTile)}, layer});

			// Second triangle of the rectangle
			_vertices.push_back({{ x + 0.5f, y + 0.5f }, {(tile_x+1)*(_xTexTile), 1.0-((tile_y - 1) *_yTexTile)}, layer});
			_vertices.push_back({{ x - 0.5f, y - 0.5f }, {tile_x*(_xTexTile), 1.0-(tile_y*_yTexTile)}, layer});
			_vertices.push_back({{ x + 0.5f, y - 0.5f }, {(tile_x+1)*(_xTexTile), 1.0-(tile_y*_yTexTile)}, layer});
		});

		// Updating MVP-Matrix and give it to the shader
		glm::mat4 MVP = cam.vp();
		_shader.bind().set_uniform("MVP", MVP)
			   .set_uniform("myTextureSampler", 0);

		// Binding tilemap texture
		_texture->bind();

		_object.buffer().set(_vertices);
		_object.draw();

		if(talkative){
			std::cout << "Drawing Tiles start at X:" << cam_start.x << " | at Y:" << cam_start.y
			          << "\nends at X:" << cam_end.x << " | at Y:" << cam_end.y << std::endl;
			std::cout << "CamPos: " << cam.position().x << "/" << cam.position().y << std::endl;
			std::cout << "cur zoom: " << cam.zoom() << std::endl;
			std::cout << "CamSize: " << cam.viewport().z << ":" << cam.viewport().w << std::endl;
		}

	}

}
}
