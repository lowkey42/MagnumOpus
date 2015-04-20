
#include "example_screen.hpp"
#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game {
	using namespace core::unit_literals;
	using namespace core::renderer;

	struct Test_vertex {
		glm::vec2 pos;
		glm::vec3 color;
	};

	Vertex_layout layout {
		Vertex_layout::Mode::triangles,
		vertex("position",  &Test_vertex::pos),
		vertex("color",     &Test_vertex::color)
	};

	auto triangle() -> std::vector<Test_vertex> {
		return {
			{{-1, -1}, {1, 0, 0}},
			{{ 1, -1}, {0, 1, 0}},
			{{ 0,  1}, {0, 0, 1}}
		};

	}


	Example_screen::Example_screen(core::Engine& engine) :
		core::Screen(engine),
	    _object(layout, create_buffer(triangle())),
	    _font(engine.assets().load<core::renderer::Font>(("font:test"_aid)))
	{
		_text_shader.attach_shader(engine.assets().load<core::renderer::Shader>(("vert_shader:simple"_aid)))
			 .attach_shader(engine.assets().load<core::renderer::Shader>(("frag_shader:simple"_aid)))
			 .bind_all_attribute_locations(text_vertex_layout)
			 .build();

		_shader.attach_shader(engine.assets().load<core::renderer::Shader>(("vert_shader:sprite"_aid)))
		       .attach_shader(engine.assets().load<core::renderer::Shader>(("frag_shader:sprite"_aid)))
		       .bind_all_attribute_locations(layout)
		       .build();
	}

	void Example_screen::_update(float delta_time) {
		static float time_acc = 0;

		_engine.assets().reload();

		time_acc+=delta_time;
		_shader.bind().set_uniform("time", time_acc);
	}

	void Example_screen::_draw(float time ) {
		_shader.bind();
		_object.draw();

		_font->bind();
		_text_shader.bind()
		            .set_uniform("model",   glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f)))
		            .set_uniform("VP",      glm::ortho(-400.f,400.f,300.f,-300.f))
		            .set_uniform("texture", 0)
		            .set_uniform("layer",   0.f)
		            .set_uniform("color",   glm::vec4(1,1,1,0.6));
		_font->text("Test_\nSatz!")->draw();
	}

}
