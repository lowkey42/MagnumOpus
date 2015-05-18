#include "sprite_batch.hpp"

namespace mo {
namespace renderer {

	using namespace renderer;

	// layout description for vertices
	Vertex_layout layout {
		Vertex_layout::Mode::triangles,
		vertex("position",  &Sprite_batch::Sprite_vertex::pos),
		vertex("uv",        &Sprite_batch::Sprite_vertex::uv)
	};

	Sprite_batch::Sprite_batch(asset::Asset_manager& asset_manager) : _object(layout, create_dynamic_buffer<Sprite_vertex>(64)){
		_shader.attach_shader(asset_manager.load<Shader>("vert_shader:sprite_batch"_aid))
			   .attach_shader(asset_manager.load<Shader>("frag_shader:sprite_batch"_aid))
		       .bind_all_attribute_locations(layout)
		       .build();
	}

	void Sprite_batch::draw(const Camera& cam, const Sprite& sprite) noexcept {

		float x = sprite.position.x.value(), y = sprite.position.y.value();
		float width = sprite.anim->frame_height() / cam.world_scale(), height = sprite.anim->frame_height() / cam.world_scale();
		glm::vec4 uv = glm::vec4(sprite.uv);

		// Rotation Matrix to be applied to coords of the Sprite
		glm::mat4 rotMat = glm::translate(glm::vec3(x, y, 0.f)) * glm::rotate(sprite.rotation -1.5707f, glm::vec3(0.f, 0.f, 1.f));

//		std::cout << "Entity with Sprite Component at: " << x << "/" << y << std::endl;
//		std::cout << "Name of attached texture: " << sprite.texture.str() << std::endl;
//		std::cout << "rotation is: " << sprite.rotation << std::endl;

		_vertices.push_back(Sprite_vertex(rotMat * glm::vec4(-width / 2.f, -height / 2.f, 0.0f, 1.0f), {uv.x, uv.y}, &*sprite.anim->texture()));
		_vertices.push_back(Sprite_vertex(rotMat * glm::vec4(-width / 2.f, height / 2.f, 0.0f, 1.0f), {uv.x, uv.w}, &*sprite.anim->texture()));
		_vertices.push_back(Sprite_vertex(rotMat * glm::vec4(width / 2.f, height / 2.f, 0.0f, 1.0f), {uv.z, uv.w}, &*sprite.anim->texture()));

		//_vertices.push_back({{x, y}, {uv.x, uv.w}, {sprite.texture}});
		//_vertices.push_back({{x, y+1.f}, {uv.x, uv.y}, {sprite.texture}});
		//_vertices.push_back({{x+1.f, y+1.f}, {uv.z, uv.y}, {sprite.texture}});

		_vertices.push_back(Sprite_vertex(rotMat * glm::vec4(width / 2.f, height / 2.f, 0.0f, 1.0f), {uv.z, uv.w}, &*sprite.anim->texture()));
		_vertices.push_back(Sprite_vertex(rotMat * glm::vec4(-width / 2.f, -height / 2.f, 0.0f, 1.0f), {uv.x, uv.y}, &*sprite.anim->texture()));
		_vertices.push_back(Sprite_vertex(rotMat * glm::vec4(width / 2.f, -height / 2.f, 0.0f, 1.0f), {uv.z, uv.y}, &*sprite.anim->texture()));

		//_vertices.push_back({{x+1.f, y+1.f}, {uv.z, uv.y}, {sprite.texture}});
		//_vertices.push_back({{x, y}, {uv.x, uv.w}, {sprite.texture}});
		//_vertices.push_back({{x+1.f, y}, {uv.z, uv.w}, {sprite.texture}});

	}


	void Sprite_batch::draw_part(std::vector<Sprite_vertex>::const_iterator begin, std::vector<Sprite_vertex>::const_iterator end){

		// check for nullptr and begin != end
		// bind the appropriate texture and set the object buffer to
		// the corresponding positions that has to be drawn
		if(begin!=end && begin->tex){
			begin->tex->bind();
			_object.buffer().set<Sprite_vertex>(begin, end);
			_object.draw();
		}

	}


	void Sprite_batch::drawAll(const Camera& cam) noexcept {

		glm::mat4 MVP = cam.vp();
		_shader.bind()
			   .set_uniform("MVP", MVP)
			   .set_uniform("myTextureSampler", 0);

		// Sorting the vertices by used texture
		// so that blocks of vertices are bound together
		// where the textures match each other -> less switching in texture binding
		std::stable_sort(_vertices.begin(), _vertices.end());

		auto last = _vertices.begin();
		for(auto curIter = _vertices.begin(); curIter != _vertices.end(); ++curIter){

			// if texture reference at the current iterator position differs from the one before
			// draw from the last marked iterator position to the current position
			// and set the last marker to the current iterator position
			if(curIter->tex != last->tex){
				draw_part(last, curIter);
				last = curIter;
			}
		}

		draw_part(last, _vertices.end());

		_vertices.clear();

	}

}
}
