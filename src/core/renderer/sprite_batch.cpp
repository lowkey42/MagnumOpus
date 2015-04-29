#include "sprite_batch.hpp"

namespace mo {
namespace renderer {

	using namespace renderer;

	bool talkative = false;

	// layout description for vertices
	Vertex_layout layout {
		Vertex_layout::Mode::triangles,
		vertex("position",  &Sprite_batch::SpriteVertex::pos),
		vertex("uv",        &Sprite_batch::SpriteVertex::uv)
	};

	Sprite_batch::Sprite_batch(asset::Asset_manager& asset_manager) : _object(layout, create_buffer(_vertices, true)){
		_shader.attach_shader(asset_manager.load<Shader>("vert_shader:sprite_batch"_aid))
			   .attach_shader(asset_manager.load<Shader>("frag_shader:sprite_batch"_aid))
		       .bind_all_attribute_locations(layout)
		       .build();
	}

	void Sprite_batch::draw(const Camera& cam, const Sprite& sprite) noexcept {

		float x = sprite.position.x.value(), y = sprite.position.y.value();
		glm::vec4 uv = glm::vec4(sprite.uv);

		sprite.texture.bind();


//		std::cout << "Entity with Sprite Component at: " << x << "/" << y << std::endl;
//		std::cout << "Name of attached texture: " << sprite.texture.str() << std::endl;

//		std::cout << "rotation is: " << sprite.rotation << std::endl;

		// WHY -1.645f?
		glm::mat4 rotMat = glm::translate(glm::vec3(x + 0.5f, y + 0.5f, 0.f)) * glm::rotate(sprite.rotation -1.645f, glm::vec3(0.f, 0.f, 1.f)) * glm::translate(-glm::vec3(x + 0.5f, y + 0.5f, 0.f));

		glm::vec4 vec1 = rotMat * glm::vec4(x, y, 0.0f, 1.0f);
		_vertices.push_back({{vec1.x, vec1.y}, {uv.x, uv.w}, {sprite.texture}});
		glm::vec4 vec2 = rotMat * glm::vec4(x, y+1.f, 0.0f, 1.0f);
		_vertices.push_back({{vec2.x, vec2.y}, {uv.x, uv.y}, {sprite.texture}});
		glm::vec4 vec3 = rotMat * glm::vec4(x+1.f, y+1.f, 0.0f, 1.0f);
		_vertices.push_back({{vec3.x, vec3.y}, {uv.z, uv.y}, {sprite.texture}});

		//_vertices.push_back({{x, y}, {uv.x, uv.w}, {sprite.texture}});
		//_vertices.push_back({{x, y+1.f}, {uv.x, uv.y}, {sprite.texture}});
		//_vertices.push_back({{x+1.f, y+1.f}, {uv.z, uv.y}, {sprite.texture}});

		if(talkative){
			// DEBUG CODE TO CHECK GIVEN UV DATA AND POS OF FIRST TRIANGLE
			std::cout << "x / y -> " << x << "/" << y << std::endl;
			std::cout << "ux / uy -> " << uv.x << "/" << uv.w << std::endl;
			std::cout << "x / y -> " << x << "/" << y+1 << std::endl;
			std::cout << "ux / uy -> " << uv.x << "/" << uv.y << std::endl;
			std::cout << "x / y -> " << x+1 << "/" << y+1 << std::endl;
			std::cout << "ux / uy -> " << uv.z << "/" << uv.y << std::endl;
		}

		_vertices.push_back({{vec3.x, vec3.y}, {uv.z, uv.y}, {sprite.texture}});
		_vertices.push_back({{vec1.x, vec1.y}, {uv.x, uv.w}, {sprite.texture}});
		glm::vec4 vec4 = rotMat * glm::vec4(x+1.f, y, 0.0f, 1.0f);
		_vertices.push_back({{vec4.x, vec4.y}, {uv.z, uv.w}, {sprite.texture}});

		//_vertices.push_back({{x+1.f, y+1.f}, {uv.z, uv.y}, {sprite.texture}});
		//_vertices.push_back({{x, y}, {uv.x, uv.w}, {sprite.texture}});
		//_vertices.push_back({{x+1.f, y}, {uv.z, uv.w}, {sprite.texture}});

	}


	void Sprite_batch::drawAll(const Camera& cam) noexcept {

		glm::mat4 MVP = cam.vp();
		_shader.bind()
			   .set_uniform("MVP", MVP)
			   .set_uniform("myTextureSampler", 0);

		_vertices.at(0).tex.bind();

		_object.buffer().set(_vertices);
		_object.draw();

		_vertices.clear();

	}

}
}
