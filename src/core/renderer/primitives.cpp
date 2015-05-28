#include "primitives.hpp"


namespace mo {
namespace renderer {

	Vertex_layout simple_vertex_layout {
		Vertex_layout::Mode::triangles,
		vertex("position", &Simple_vertex::xy),
		vertex("uv", &Simple_vertex::uv)
	};

	namespace {
		struct Ray_vertex {
			glm::vec2 pos;
		};

		auto ray_layout = renderer::Vertex_layout{
			renderer::Vertex_layout::Mode::triangle_strip,
			renderer::vertex("position", &Ray_vertex::pos)
		};
		auto ray_mesh = std::vector<Ray_vertex>{
			{{0,0}}, {{1,0}}, {{0,1}}, {{1,1}}
		};
	}

	Ray_renderer::Ray_renderer(asset::Asset_manager& assets)
		: _obj(ray_layout, create_buffer(ray_mesh)) {

		_prog.attach_shader(assets.load<Shader>("frag_shader:ray"_aid))
			 .attach_shader(assets.load<Shader>("vert_shader:ray"_aid))
			 .bind_all_attribute_locations(ray_layout)
			 .build()
			 .bind()
			 .set_uniform("color", glm::vec4(1,0,0,0.5));
	}

	void Ray_renderer::set_vp(const glm::mat4& vp) {
		_prog.bind().set_uniform("vp", vp);
	}

	void Ray_renderer::draw(glm::vec3 p, Angle a, float length, glm::vec4 color, float width) {
		auto scale = glm::scale(glm::translate(glm::mat4(), glm::vec3(0,-width/2.f,0)),
								glm::vec3(length, width, 1));
		auto rot = glm::rotate(a.value(), glm::vec3(0,0,1));
		auto trans = glm::translate(glm::mat4(), p);

		_prog.bind()
				.set_uniform("model", trans*rot*scale)
		        .set_uniform("color", color);
		_obj.draw();
	}

}
}
