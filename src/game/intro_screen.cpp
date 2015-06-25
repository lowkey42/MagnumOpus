#include "intro_screen.hpp"

#include "../core/units.hpp"
#include "../core/renderer/graphics_ctx.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <game/game_screen.hpp>
#include <game/example_screen.hpp>


namespace mo {
	using namespace unit_literals;
	using namespace renderer;

	struct Intro_vertex {
		glm::vec2 pos;
		glm::vec2 uv;
	};

	Vertex_layout intro_layout {
		Vertex_layout::Mode::triangles,
		vertex("position",  &Intro_vertex::pos),
		vertex("in_uv",     &Intro_vertex::uv)
	};

	auto intro_quad() -> std::vector<Intro_vertex> {
		return {
			// Quad Part left
			{{-1, 1}, {0, 1}},
			{{-1, -1}, {0, 0}},
			{{1, -1}, {1, 0}},

			// Quad Part right
			{{1, -1}, {1, 0}},
			{{-1, 1}, {0, 1}},
			{{1, 1}, {1, 1}}
		};
	}

	Intro_screen::Intro_screen(Engine& engine) :
		Screen(engine),
		_object(intro_layout, create_buffer(intro_quad()))
	{
		_shader.attach_shader(engine.assets().load<Shader>(("vert_shader:intro"_aid)))
			   .attach_shader(engine.assets().load<Shader>(("frag_shader:intro"_aid)))
			   .bind_all_attribute_locations(intro_layout)
			   .build();

		_texture = _engine.assets().load<Texture>("tex:ui_intro"_aid);
	}

	void Intro_screen::_update(float delta_time) {

		SDL_Event event;
		bool switchScreen = false;
		while(SDL_PollEvent(&event)){
			switchScreen = true;
		}
		if(switchScreen)
			_engine.enter_screen<Example_screen>();
			// not working -> why? :(
			// _engine.enter_screen<Game_screen>("default", std::vector<ecs::ETO>{}, util::just(0));
	}

	void Intro_screen::_draw(float time ) {

		glm::mat4 VP = glm::ortho(-1.f, 1.f, -1.f, 1.f);
		_shader.bind().set_uniform("MVP", VP)
				.set_uniform("myTextureSampler", 0);
		_texture->bind();
		_object.draw();

	}
}
