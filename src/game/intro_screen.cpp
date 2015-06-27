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

	namespace {

		struct ui_controller : sys::controller::Controllable_interface{

			ui_controller(Game_engine& engine) : _engine(engine){}

			void move(glm::vec2 direction) override {}
			void look_at(glm::vec2 position) override {}
			void look_in_dir(glm::vec2 direction) override {}
			void attack() override { enter_game(); }
			void use() override { enter_game(); }
			void take() override { enter_game(); }
			void switch_weapon(uint32_t weapon_id) override { enter_game(); }

			void enter_game(){
			 _engine.enter_screen<Game_screen>("default", std::vector<ecs::ETO>{}, util::just(0));
			}

			Game_engine& _engine;

		};

	}

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
			{{-1, -1}, {0, 1}},
			{{-1, 1}, {0, 0}},
			{{1, 1}, {1, 0}},

			// Quad Part right
			{{1, 1}, {1, 0}},
			{{-1, -1}, {0, 1}},
			{{1, -1}, {1, 1}}
		};
	}


	Intro_screen::Intro_screen(Game_engine& game_engine) :
		Screen(game_engine),
		_game_engine(game_engine),
		_object(intro_layout, create_buffer(intro_quad()))
	{
		_shader.attach_shader(_engine.assets().load<Shader>(("vert_shader:simple"_aid)))
			   .attach_shader(_engine.assets().load<Shader>(("frag_shader:simple"_aid)))
			   .bind_all_attribute_locations(intro_layout)
			   .build();

		_texture = _engine.assets().load<Texture>("tex:ui_intro"_aid);
		_camera.reset(new renderer::Camera(glm::vec2(2, 2)));
	}


	void Intro_screen::_update(float delta_time) {

		ui_controller uic(_game_engine);
		_game_engine.controllers().main_controller()(uic);

	}


	void Intro_screen::_draw(float time ) {

		_shader.bind().set_uniform("VP", _camera->vp())
				.set_uniform("model",   glm::mat4(1.0f))
				.set_uniform("texture", 0)
				.set_uniform("layer",   0.f)
				.set_uniform("color",   glm::vec4(1,1,1,0.6));
		_texture->bind();
		_object.draw();

	}
}
