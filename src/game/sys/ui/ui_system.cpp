#include "ui_system.hpp"

#include <core/renderer/primitives.hpp>

#include "../combat/health_comp.hpp"
#include "../combat/score_comp.hpp"

#include <sstream>
#include <iomanip>

namespace mo {
namespace sys {
namespace ui {

	using namespace renderer;

	namespace {
		std::vector<Simple_vertex> hud_vert {
			{{0,0}, {0,1}},
			{{0,1}, {0,0}},
			{{1,0}, {1,1}},

			{{1,1}, {1,0}},
			{{1,0}, {1,1}},
			{{0,1}, {0,0}}
		};
	}

	Ui_system::Ui_system(Engine& e, ecs::Entity_manager& em)
	    : _ui_comps(em.list<Ui_comp>()),
	      _cam(calculate_vscreen(e, 512)),
	      _hud(simple_vertex_layout, create_buffer(hud_vert)),
	      _hud_bg_tex(e.assets().load<Texture>("tex:ui_hud_bg"_aid)),
	      _hud_fg_tex(e.assets().load<Texture>("tex:ui_hud_fg"_aid)),
	      _hud_health_tex(e.assets().load<Texture>("tex:ui_hud_health"_aid)),
	      _score_font(e.assets().load<Font>("font:nixie"_aid)),
	      _score_text(_score_font){

		_score_shader.attach_shader(e.assets().load<renderer::Shader>("vert_shader:simple"_aid))
		             .attach_shader(e.assets().load<renderer::Shader>("frag_shader:simple"_aid))
                     .bind_all_attribute_locations(renderer::text_vertex_layout)
                     .build();

		_hud_shader.attach_shader(e.assets().load<Shader>("vert_shader:hud"_aid))
	               .attach_shader(e.assets().load<Shader>("frag_shader:hud"_aid))
	               .bind_all_attribute_locations(simple_vertex_layout)
	               .build();

		_cam.zoom(0.5f);
	}

	void Ui_system::update(Time dt) {
		// TODO
	}

	void Ui_system::draw() {
		renderer::Disable_depthtest ddt{};
		(void)ddt;

		auto screen_w = _cam.viewport().z * 1/_cam.zoom();
		auto screen_h = _cam.viewport().w * 1/_cam.zoom();

		auto w = _hud_bg_tex->width();
		auto h = _hud_bg_tex->height();

		_hud_bg_tex->bind(0);
		_hud_fg_tex->bind(1);
		_hud_health_tex->bind(2);

		_hud_shader.bind()
				   .set_uniform("bg_tex", 0)
				   .set_uniform("fg_tex", 1)
				   .set_uniform("health_tex", 2);

		_hud_bg_tex->bind(0);
		_hud_fg_tex->bind(1);
		_hud_health_tex->bind(2);

		for(auto& hud : _ui_comps) {
			auto model = glm::scale(glm::translate(glm::mat4{}, glm::vec3(-screen_w/2+10, screen_h/2-h-10, 0)), glm::vec3(w,h,1));

			_hud_shader.set_uniform("mvp", _cam.vp()*model)
			           .set_uniform("health",
			                        hud.owner().get<combat::Health_comp>().process(1.f,
			                                                                       [](auto& h){return h.hp_percent();}));

			_hud.draw();

			// TODO[foe]: draw element indicators
		}

		_hud_bg_tex->unbind(0);
		_hud_fg_tex->unbind(1);
		_hud_health_tex->unbind(2);


		_score_font->bind();
		_score_shader.bind()
		             .set_uniform("VP", _cam.vp())
		             .set_uniform("texture", 0)
		             .set_uniform("layer",   1.0f)
		             .set_uniform("color",   glm::vec4(2,2,2,1));

		for(auto& hud : _ui_comps) {
			hud.owner().get<sys::combat::Score_comp>().process([&](const auto& s){
				std::stringstream str;
				str<<std::setfill(' ')<<std::setw(4)<<s.value();

				_score_text.set(str.str());


				auto model = glm::translate(glm::mat4{}, glm::vec3(-screen_w/2+10  +250, screen_h/2-h-10 +64+27, 0));
				_score_shader.set_uniform("model",model);
				_score_text.draw();
			});
		}
	}

}
}
}
