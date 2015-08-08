#define GLM_SWIZZLE

#include "ui_system.hpp"

#include "ui_minmal_comp.hpp"

#include <core/renderer/primitives.hpp>

#include "../combat/comp/health_comp.hpp"
#include "../combat/comp/score_comp.hpp"
#include "../item/element_comp.hpp"

#include <sstream>
#include <iomanip>

namespace mo {
namespace sys {
namespace ui {

	using namespace renderer;

	namespace {
		constexpr auto gui_delay = 0.2f;
		constexpr auto gui_delay_inv = 1.f / gui_delay;

		const std::vector<Simple_vertex> hud_vert {
			{{0,0}, {0,1}},
			{{0,1}, {0,0}},
			{{1,0}, {1,1}},

			{{1,1}, {1,0}},
			{{1,0}, {1,1}},
			{{0,1}, {0,0}}
		};

		const glm::vec2 element_offsets[] = {
		    glm::vec2{128.f, 5.f+58/2.f},
		    glm::vec2{222.f, 130},
		    glm::vec2{128.f, 222},
		    glm::vec2{32.f, 130}
		};

		auto element_texture(asset::Asset_manager& a, level::Element e) -> Texture_ptr {
			switch(e) {
				case level::Element::neutral:
					return {};

				case level::Element::fire:
					return a.load<Texture>("tex:element_fire"_aid);

				case level::Element::frost:
					return a.load<Texture>("tex:element_frost"_aid);

				case level::Element::water:
					return a.load<Texture>("tex:element_water"_aid);

				case level::Element::stone:
					return a.load<Texture>("tex:element_stone"_aid);

				case level::Element::gas:
					return a.load<Texture>("tex:element_gas"_aid);

				case level::Element::lightning:
					return a.load<Texture>("tex:element_lightning"_aid);

            }
            FAIL("Unreachable code");
        }
	}

	Ui_system::Ui_system(Engine& e, ecs::Entity_manager& em, physics::Transform_system& transforms)
	    : _ui_comps(em.list<Ui_comp>()),
	      _cam(calculate_vscreen(e, 512)),
	      _hud(simple_vertex_layout, create_buffer(hud_vert)),
	      _hud_bg_tex(e.assets().load<Texture>("tex:ui_hud_bg"_aid)),
	      _hud_fg_tex(e.assets().load<Texture>("tex:ui_hud_fg"_aid)),
	      _hud_health_tex(e.assets().load<Texture>("tex:ui_hud_health"_aid)),
	      _hud_health_min_tex(e.assets().load<Texture>("tex:ui_hud_health_min"_aid)),
	      _score_font(e.assets().load<Font>("font:nixie"_aid)),
	      _score_text(_score_font),
	      _bubble_renderer(e.assets(), 58/2.f),
	      _assets(e.assets()),
	      _transforms(transforms) {

		em.register_component_type<Ui_minimal_comp>();

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


		int i = 0;
		for(auto& hud : _ui_comps) {
			auto current_health = hud.owner().get<combat::Health_comp>()
			                                 .process(0.f, [](auto& h){return h.hp_percent();});

			auto current_score = hud.owner().get<sys::combat::Score_comp>()
			                                .process(0, [&](const auto& s){return s.value();});

			std::array<bool,       4> element_active;
			std::array<float,      4> element_fill;
			std::array<Texture_ptr,4> element_tex;
			hud.owner().get<item::Element_comp>().process([&](auto& e){
				for(auto i : util::range(4)) {
					const auto& slot = e.slot(i);
					element_active[i] = slot.active;
					element_fill[i] = slot.fill;
					element_tex[i] = element_texture(_assets, slot.element);
				}
			});

			if(!hud._initialized) {
				hud._initialized = true;

				for(auto i : util::range(4)) {
					auto& e = hud._elements[i];

					e.activity = element_active[i] ? 1 : 0;
					e.fill     = element_fill[i];
					e.texture  = element_tex[i];
				}

				hud._health = current_health;
				hud._health_c = current_health;
				hud._score = current_score;

				auto offset = calc_offset(i);
				hud._target_offset = offset;
				hud._offset = offset + glm::vec3(glm::sign(offset.x)*w, 0, 0);

			} else {
				auto lerp = [dt](auto& a, auto b) {
					auto t = glm::clamp(gui_delay_inv * dt.value(), 0.01f, 1.f) * 0.5f;

					a = a*(1-t) + b*t;
				};

				for(auto i : util::range(4)) {
					auto& e = hud._elements[i];

					lerp(e.activity, element_active[i] ? 1.f : 0.f);
					lerp(e.fill,     element_fill[i]);
					e.texture  = element_tex[i];
				}

				lerp(hud._health, current_health);
				hud._health_c = current_health;
				hud._score = current_score;
				lerp(hud._offset, hud._target_offset);
			}

			i++;
		}
	}

	void Ui_system::draw(const renderer::Camera& world_cam) {
		renderer::Disable_depthtest ddt{};
		(void)ddt;

		auto w = _hud_bg_tex->width();
		auto h = _hud_bg_tex->height();

		for(auto& hud : _ui_comps) {
			auto model = glm::scale(glm::translate(glm::mat4{}, hud._offset), glm::vec3(w,h,1));

			if(hud._offset.x>0)
				model = glm::scale(model, glm::vec3(-1,1,1));

			hud._mvp = _cam.vp()*model;
		}

		glm::vec2 upper_left  = world_cam.screen_to_world({world_cam.viewport().x, world_cam.viewport().y});
		glm::vec2 lower_right = world_cam.screen_to_world({world_cam.viewport().z, world_cam.viewport().w});

		_health_shader.bind().set_uniform("tex", 0);
		_hud_health_min_tex->bind();
		auto min_health_scale = glm::vec3(
			_hud_health_min_tex->width()  / world_cam.world_scale() /2.f,
			_hud_health_min_tex->height() / world_cam.world_scale() /2.f,
			1
		);

		_transforms.foreach_in_rect(upper_left, lower_right, [&](ecs::Entity& entity) {
			if(!entity.has<Ui_minimal_comp>())
				return;

			process(entity.get<physics::Transform_comp>(),
			        entity.get<combat::Health_comp>())
            >> [&](const auto& trans, const auto& health) {

				if(health.damaged()) {
					auto pos = remove_units(trans.position());

					auto model = glm::scale(glm::translate(glm::mat4{}, {pos.x-min_health_scale.x/2, pos.y-min_health_scale.y, 0.f}), min_health_scale);

					_health_shader.set_uniform("mvp", world_cam.vp() * model)
								  .set_uniform("health", health.hp_percent())
								  .set_uniform("health_anim", health.hp_percent());

					_hud.draw();
				}

			};
		});

		// draw bg
		_hud_bg_tex->bind(0);
		_hud_shader.bind().set_uniform("tex", 0);
		for(auto& hud : _ui_comps) {
			_hud_shader.set_uniform("mvp", hud._mvp);

			_hud.draw();
		}


		// draw bubbles
		_bubble_renderer.set_vp(_cam.vp());
		for(auto& hud : _ui_comps) {
			int i=-1;
			for(const auto& e : hud._elements) {
				i++;

				if(!e.texture)
					continue;

				auto e_offset = element_offsets[i];

				if(hud._offset.x>0) {
					if(i%2!=0)
						e_offset = element_offsets[(i+2) % 4];

					e_offset.x*=-1;
				}

				_bubble_renderer.draw(hud._offset.xy()+e_offset,
				                      e.fill, e.activity, _time_acc.value() + i/2.f,
				                      *e.texture);
			}
		}


		// draw health
		_health_shader.bind().set_uniform("tex", 0);
		_hud_health_tex->bind();
		for(auto& hud : _ui_comps) {
			_health_shader.set_uniform("mvp", hud._mvp)
			              .set_uniform("health", hud._health_c)
			              .set_uniform("health_anim", hud._health);

			_hud.draw();
		}


		// draw fg
		_hud_fg_tex->bind(0);
		_hud_shader.bind().set_uniform("tex", 0);
		for(auto& hud : _ui_comps) {
			_hud_shader.set_uniform("mvp", hud._mvp);

			_hud.draw();
		}

		// draw bubble-glow
		_bubble_renderer.set_vp(_cam.vp());
		for(auto& hud : _ui_comps) {
			int i=-1;
			for(const auto& e : hud._elements) {
				i++;

				if(!e.texture)
					continue;

				auto e_offset = element_offsets[i];

				if(hud._offset.x>0) {
					if(i%2!=0)
						e_offset = element_offsets[(i+2) % 4];

					e_offset.x*=-1;
				}

				_bubble_renderer.draw_glow(hud._offset.xy()+e_offset,
				                      e.fill, e.activity, _time_acc.value() + i/2.f,
				                      *e.texture);
			}
		}


		// draw score
		_score_font->bind();
		_score_shader.bind()
		             .set_uniform("VP", _cam.vp())
		             .set_uniform("texture", 0)
		             .set_uniform("layer",   1.0f)
		             .set_uniform("color",   glm::vec4(2,2,2,1));

		for(auto& hud : _ui_comps) {
			std::stringstream str;
			str<<std::setfill(' ')<<std::setw(4)<<hud._score;

			_score_text.set(str.str());

			auto offset = hud._offset;

			if(offset.x>0)
				offset+=glm::vec3(-250-180, 64+27, 0);
			else
				offset+=glm::vec3(+250, 64+27, 0);

			auto model = glm::translate(glm::mat4{}, offset);

			_score_shader.set_uniform("model",model);
			_score_text.draw();
		}
	}

}
}
}
