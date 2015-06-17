#define GLM_SWIZZLE

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
	      _score_text(_score_font),
	      _bubble_renderer(e.assets(), 58/2.f),
	      _assets(e.assets()) {

		_score_shader.attach_shader(e.assets().load<renderer::Shader>("vert_shader:simple"_aid))
		             .attach_shader(e.assets().load<renderer::Shader>("frag_shader:simple"_aid))
                     .bind_all_attribute_locations(renderer::text_vertex_layout)
                     .build();

		_hud_shader.attach_shader(e.assets().load<Shader>("vert_shader:hud"_aid))
	               .attach_shader(e.assets().load<Shader>("frag_shader:hud"_aid))
	               .bind_all_attribute_locations(simple_vertex_layout)
	               .build();

		_health_shader.attach_shader(e.assets().load<Shader>("vert_shader:hud_health"_aid))
		              .attach_shader(e.assets().load<Shader>("frag_shader:hud_health"_aid))
                      .bind_all_attribute_locations(simple_vertex_layout)
                      .build();

		_cam.zoom(0.5f);
	}

	void Ui_system::update(Time dt) {
		_time_acc+=dt;
		// TODO: animations ?
	}

	void Ui_system::draw() { // TODO: refactor
		renderer::Disable_depthtest ddt{};
		(void)ddt;

		auto screen_w = _cam.viewport().z * 1/_cam.zoom();
		auto screen_h = _cam.viewport().w * 1/_cam.zoom();

		auto w = _hud_bg_tex->width();
		auto h = _hud_bg_tex->height();

		auto calc_offset = [&](int idx) {
			switch(idx) {
				case 0:   return glm::vec3(-screen_w/2+10, screen_h/2-h-10, 0);
				case 1:   return glm::vec3(+screen_w/2-10, -screen_h/2+10, 0);
				case 2:   return glm::vec3(-screen_w/2+10,   -screen_h/2+10, 0);
				case 3:   return glm::vec3(+screen_w/2-10, +screen_h/2-h-10, 0);
				default:  return glm::vec3(0,0,-1000); //< hide
			}
		};

		// draw bg
		_hud_bg_tex->bind(0);
		_hud_shader.bind().set_uniform("tex", 0);
		int idx = 0;
		for(auto& hud : _ui_comps) {
			(void)hud;
			auto offset = calc_offset(idx++);

			auto model = glm::scale(glm::translate(glm::mat4{}, offset), glm::vec3(w,h,1));

			if(offset.x>0)
				model = glm::scale(model, glm::vec3(-1,1,1));

			_hud_shader.set_uniform("mvp", _cam.vp()*model);

			_hud.draw();
		}

		// draw elements
		auto element_offsets = {
		    glm::vec2{128.f, 5.f+58/2.f},
		    glm::vec2{222.f, 130},
		    glm::vec2{128.f, 222},
		    glm::vec2{32.f, 130}
		};
		_bubble_renderer.set_vp(_cam.vp());
		idx = 0;
		for(auto& hud : _ui_comps) {
			(void)hud;
			auto offset = calc_offset(idx++);

			float fill=0;
			// TODO[foe]: draw element indicators
			int element_num = 0;
			for(auto& o : element_offsets) {
				float activity = 1;
				_bubble_renderer.draw(offset.xy()+o, fill, activity, _time_acc.value() + element_num/2.f,
									  *_assets.load<renderer::Texture>("tex:bubble_test.tga"_aid));
				fill+=1.f / 3;
				element_num++;
			}

		}

		// draw health
		_health_shader.bind().set_uniform("tex", 0);
		_hud_health_tex->bind();
		idx = 0;
		for(auto& hud : _ui_comps) {
			auto offset = calc_offset(idx++);

			auto model = glm::scale(glm::translate(glm::mat4{}, offset), glm::vec3(w,h,1));

			if(offset.x>0)
				model = glm::scale(model, glm::vec3(-1,1,1));

			_health_shader.set_uniform("mvp", _cam.vp()*model)
			              .set_uniform("health",
			                           hud.owner().get<combat::Health_comp>().process(1.f,
			                                                                       [](auto& h){return h.hp_percent();}));

			_hud.draw();
		}

		// draw fg
		_hud_fg_tex->bind(0);
		_hud_shader.bind().set_uniform("tex", 0);
		idx = 0;
		for(auto& hud : _ui_comps) {
			(void)hud;
			auto offset = calc_offset(idx++);

			auto model = glm::scale(glm::translate(glm::mat4{}, offset), glm::vec3(w,h,1));

			if(offset.x>0)
				model = glm::scale(model, glm::vec3(-1,1,1));

			_hud_shader.set_uniform("mvp", _cam.vp()*model);

			_hud.draw();
		}


		// draw score
		_score_font->bind();
		_score_shader.bind()
		             .set_uniform("VP", _cam.vp())
		             .set_uniform("texture", 0)
		             .set_uniform("layer",   1.0f)
		             .set_uniform("color",   glm::vec4(2,2,2,1));

		idx = 0;
		for(auto& hud : _ui_comps) {
			auto offset = calc_offset(idx++);

			hud.owner().get<sys::combat::Score_comp>().process([&](const auto& s){
				std::stringstream str;
				str<<std::setfill(' ')<<std::setw(4)<<s.value();

				_score_text.set(str.str());

				if(offset.x>0)
					offset+=glm::vec3(-250-180, 64+27, 0);
				else
					offset+=glm::vec3(+250, 64+27, 0);

				auto model = glm::translate(glm::mat4{}, offset);

				_score_shader.set_uniform("model",model);
				_score_text.draw();
			});
		}
	}

}
}
}
