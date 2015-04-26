#define GLM_SWIZZLE

#include "tilemap.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <math.h>

#include "level.hpp"

namespace game {
namespace level {

	bool talkative = false;

	// layout description for vertices
	core::renderer::Vertex_layout layout {
		core::renderer::Vertex_layout::Mode::triangles,
		core::renderer::vertex("position",	&Tilemap::TileVertex::pos),
		core::renderer::vertex("uv",		&Tilemap::TileVertex::uv),
		core::renderer::vertex("layer",		&Tilemap::TileVertex::layer)
	};

	Tilemap::Tilemap(core::Engine &engine, const game::level::Level &lev)
	    : _level(lev),
	      _object(layout, core::renderer::create_buffer(_vertices, true)) {
		// Create and attach the Shader
		_shader.attach_shader(engine.assets().load<core::renderer::Shader>("vert_shader:tilemap"_aid))
		        .attach_shader(engine.assets().load<core::renderer::Shader>("frag_shader:tilemap"_aid))
		        .bind_all_attribute_locations(layout)
		        .build();

		// Load a predefined texture and bind it
		_texture = engine.assets().load<core::renderer::Texture>("tex:tilemap_m"_aid);
	}


	void Tilemap::draw(const core::renderer::Camera& cam){
		auto cam_start = cam.screen_to_world(cam.viewport().xy()) - glm::vec2(1,1);
		auto cam_end   = cam.screen_to_world(cam.viewport().xy() +
		                                     cam.viewport().zw()) + glm::vec2(1,1);

		auto cam_range = glm::abs(cam_end-cam_start);
		_vertices.clear();
		_vertices.reserve(std::ceil(cam_range.x * cam_range.y)*6);

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
				layer = 1;

			// First triangle of the rectangle
			_vertices.push_back({{ x, y }, {tile_x*(_xTexTile), 1.0-(tile_y*_yTexTile)}, layer});
			_vertices.push_back({{ x, 1 + y }, {tile_x*(_xTexTile), 1.0-((tile_y - 1) *_yTexTile)} , layer});
			_vertices.push_back({{ 1 + x, 1 + y }, {(tile_x+1)*(_xTexTile), 1.0-((tile_y - 1) *_yTexTile)}, layer});

			// Second triangle of the rectangle
			_vertices.push_back({{ 1 + x, 1 + y }, {(tile_x+1)*(_xTexTile), 1.0-((tile_y - 1) *_yTexTile)}, layer});
			_vertices.push_back({{ x, y }, {tile_x*(_xTexTile), 1.0-(tile_y*_yTexTile)}, layer});
			_vertices.push_back({{ 1 + x, y }, {(tile_x+1)*(_xTexTile), 1.0-(tile_y*_yTexTile)}, layer});
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
