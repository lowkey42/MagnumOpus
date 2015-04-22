#include "sprite_batch.hpp"

namespace core {
namespace renderer {

	bool talkative = false;

	// layout description for vertices
	core::renderer::Vertex_layout layout {
		core::renderer::Vertex_layout::Mode::triangles,
		core::renderer::vertex("position",  &Sprite_batch::TileVertex::pos),
		core::renderer::vertex("uv",     &Sprite_batch::TileVertex::uv)
	};

	Sprite_batch::Sprite_batch(core::asset::Asset_manager& asset_manager) : _asset_manager(asset_manager), _object(layout, core::renderer::create_buffer(_vertices, true)){
		_shader.attach_shader(asset_manager.load<core::renderer::Shader>("vert_shader:sprite_batch"_aid))
		       .attach_shader(asset_manager.load<core::renderer::Shader>("frag_shader:sprite_batch"_aid))
		       .bind_all_attribute_locations(layout)
		       .build();
	}

	void Sprite_batch::draw(const core::renderer::Camera& cam, Sprite& sprite) noexcept {

		float x = sprite.position.x.value(), y = sprite.position.y.value();
		glm::vec4 uv = glm::vec4(sprite.uv);

		std::cout << "Entity with Sprite Component at: " << x << "/" << y << std::endl;
		std::cout << "Name of attached texture: " << sprite.texture.str() << std::endl;

		_vertices.push_back({{x, y}, {uv.x, uv.w}});
		_vertices.push_back({{x, y+1.f}, {uv.x, uv.y}});
		_vertices.push_back({{x+1.f, y+1.f}, {uv.z, uv.y}});

		if(talkative){
			// DEBUG CODE TO CHECK GIVEN UV DATA AND POS OF FIRST TRIANGLE
			std::cout << "x / y -> " << x << "/" << y << std::endl;
			std::cout << "ux / uy -> " << uv.x << "/" << uv.w << std::endl;
			std::cout << "x / y -> " << x << "/" << y+1 << std::endl;
			std::cout << "ux / uy -> " << uv.x << "/" << uv.y << std::endl;
			std::cout << "x / y -> " << x+1 << "/" << y+1 << std::endl;
			std::cout << "ux / uy -> " << uv.z << "/" << uv.y << std::endl;
		}

		_vertices.push_back({{x+1.f, y+1.f}, {uv.z, uv.y}});
		_vertices.push_back({{x, y}, {uv.x, uv.w}});
		_vertices.push_back({{x+1.f, y}, {uv.z, uv.w}});

		std::cout << "vertices size: " << _vertices.size() << std::endl;

	}


	void Sprite_batch::drawAll(const core::renderer::Camera& cam) noexcept {

		glm::mat4 MVP = cam.vp();
		_shader.set_uniform("MVP", MVP)
		       .set_uniform("myTextureSampler", 0)
		       .bind();
		if(_texture)
			_texture->bind();

		_object.buffer().set(_vertices);
		_object.draw();

		_vertices.clear();

	}

}
}
