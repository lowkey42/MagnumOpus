#include "game_ui.hpp"

#include <core/renderer/graphics_ctx.hpp>
#include <core/renderer/shader.hpp>
#include <core/renderer/text.hpp>
#include <core/renderer/camera.hpp>
#include <core/ecs/ecs.hpp>

#include "game_engine.hpp"

#include "sys/combat/health_comp.hpp"
#include "sys/combat/score_comp.hpp"


namespace mo {

	struct Game_ui::PImpl {
		renderer::Camera ui_cam;
		renderer::Font_ptr hud_font;
		renderer::Shader_program hud_text_shader;
		renderer::Text_dynamic hud_text;
		// TODO

		PImpl(Game_engine& engine)
		    : ui_cam(engine),
		      hud_font(engine.assets().load<renderer::Font>("font:test"_aid)),
		      hud_text(hud_font) {
			  hud_text_shader.attach_shader(engine.assets().load<renderer::Shader>("vert_shader:simple"_aid))
			                 .attach_shader(engine.assets().load<renderer::Shader>("frag_shader:simple"_aid))
			                 .bind_all_attribute_locations(renderer::text_vertex_layout)
			                 .build();
		}
	};

	Game_ui::Game_ui(Game_engine& engine)
	    : _impl(std::make_unique<PImpl>(engine)) {
		// TODO
	}

	Game_ui::~Game_ui()noexcept = default;

	void Game_ui::pre_draw() {
	}

	void Game_ui::draw(const renderer::Camera& cam, ecs::Entity& entity, int offset) {
		renderer::Disable_depthtest ddt{};

		_impl->hud_text_shader.bind();
		_impl->hud_font->bind();

		//_impl->ui_cam.viewport(cam.viewport());
		_impl->hud_text_shader.set_uniform("VP", _impl->ui_cam.vp())
		                      .set_uniform("texture", 0)
		                      .set_uniform("layer",   1.0f)
		                      .set_uniform("color",   glm::vec4(1,1,1,1));

		entity.get<sys::combat::Health_comp>().process([&](const auto& hc){
			_impl->hud_text.set("Health: "+util::to_string(hc.hp())+"/"
			                              +util::to_string(hc.max_hp()));

			_impl->hud_text_shader.set_uniform("model",
			                                   glm::scale(glm::translate(glm::mat4(),
			                                                  glm::vec3(-380,-260,0)), glm::vec3(0.5f, 0.5f, 1)));
			_impl->hud_text.draw();
		});

		entity.get<sys::combat::Score_comp>().process([&](const auto& s){
			_impl->hud_text.set("Score: "+util::to_string(s.value()));

			_impl->hud_text_shader.set_uniform("model",
			                                   glm::scale(glm::translate(glm::mat4(),
			                                                  glm::vec3(-380,-230,0)), glm::vec3(0.5f, 0.5f, 1)));
			_impl->hud_text.draw();
		});

	}

}
