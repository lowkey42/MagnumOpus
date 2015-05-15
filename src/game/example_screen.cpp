
#include "example_screen.hpp"
#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mo {
	using namespace unit_literals;
	using namespace renderer;

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

	Text_ptr t;


	Example_screen::Example_screen(Engine& engine) :
		Screen(engine),
	    _object(layout, create_buffer(triangle())),
		_font(engine.assets().load<Font>(("font:test"_aid)))
	{
		_text_shader.attach_shader(engine.assets().load<Shader>(("vert_shader:simple"_aid)))
			 .attach_shader(engine.assets().load<Shader>(("frag_shader:simple"_aid)))
			 .bind_all_attribute_locations(text_vertex_layout)
			 .build();

		_shader.attach_shader(engine.assets().load<Shader>(("vert_shader:sprite"_aid)))
			   .attach_shader(engine.assets().load<Shader>(("frag_shader:sprite"_aid)))
		       .bind_all_attribute_locations(layout)
		       .build();


		std::string s;
		for(int i=0; i<1000; i++){
			for(int i=0; i<100; i++)
				s+="X";

			s+="\n";
		}

		t = _font->text(s);
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
		            .set_uniform("model",   glm::scale(glm::mat4(), glm::vec3(0.2f, 0.2f, 1.f)))
		            .set_uniform("VP",      glm::ortho(-1000.f,1000.f,1000.f,-1000.f))
		            .set_uniform("texture", 0)
		            .set_uniform("layer",   0.f)
		            .set_uniform("color",   glm::vec4(1,1,1,0.6));


		t->draw();
	}

}
