#define GLM_SWIZZLE

#include "tilemap.hpp"

#include <core/engine.hpp>
#include <core/renderer/camera.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <math.h>

#include "level.hpp"

namespace mo {
namespace level {

	using namespace renderer;
	using namespace glm;

	namespace {
		constexpr bool debug_logging = false;

		constexpr auto vertex_count = 6;

		const vec3 vp[] {
			{-0.5f, -0.5f, 0},
			{-0.5f, +0.5f, 0},
			{+0.5f, -0.5f, 0},

			{+0.5f, +0.5f, 0},
			{+0.5f, -0.5f, 0},
			{-0.5f, +0.5f, 0}
		};
		const vec2 vuv[] {
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 1.0f},

			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f}
		};

	}


	// layout description for vertices
	Vertex_layout layout {
		Vertex_layout::Mode::triangles,
		vertex("position",	&TileVertex::pos),
		vertex("uv",		&TileVertex::uv)
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
	}


	void Tilemap::draw(const Camera& cam){
		auto tex_res = vec2{_texture->width(), _texture->height()};

		// Binding tilemap texture
		_texture->bind();

		auto tile_res = cam.world_scale();
		auto tileset_width = static_cast<int>(tex_res.x / cam.world_scale());
		auto pixel_correction = 0.5f;

		auto cam_area  = cam.area();
		auto cam_start = vec2{cam_area.x, cam_area.y};
		auto cam_end   = vec2{cam_area.z, cam_area.w};

		auto cam_range = glm::abs(cam_end-cam_start);
		_vertices.clear();
		_vertices.reserve(std::ceil(cam_range.x * cam_range.y) * vertex_count);

		// Filling the vector with vertices of the displayed triangles and corresponding uv-data
		_level.foreach_tile(cam_start.x, cam_start.y, cam_end.x, cam_end.y,
		                    [&](int x, int y, const auto& tile) {

			int tile_type = static_cast<int>(tile.type);
			auto tileset_pos = vec2 {
				(tile_type % tileset_width) * tile_res,
				0
			};

			auto p  = vec3{x,y, tile.height()};

			for(auto i : util::range(vertex_count)) {
				auto cor = vec2 {
					vuv[i].x>0 ? -pixel_correction : pixel_correction,
					vuv[i].y>0 ? -pixel_correction : pixel_correction
				};
				auto uv = ((vuv[i]*tile_res + tileset_pos + cor)/tex_res);

				_vertices.emplace_back(vp[i]+p, uv);
			}
		});


		// Updating MVP-Matrix and give it to the shader
		glm::mat4 MVP = cam.vp();
		_shader.bind().set_uniform("MVP", MVP)
		              .set_uniform("myTextureSampler", 0);

		_object.buffer().set(_vertices);
		_object.draw();

		if(debug_logging){
			std::cout << "Drawing Tiles start at X:" << cam_start.x << " | at Y:" << cam_start.y
			          << "\nends at X:" << cam_end.x << " | at Y:" << cam_end.y << std::endl;
			std::cout << "CamPos: " << cam.position().x << "/" << cam.position().y << std::endl;
			std::cout << "cur zoom: " << cam.zoom() << std::endl;
			std::cout << "CamSize: " << cam.viewport().z << ":" << cam.viewport().w << std::endl;
		}

	}

}
}
