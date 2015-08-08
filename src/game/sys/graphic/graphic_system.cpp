#include "graphic_system.hpp"

#include <core/units.hpp>

#include "../physics/physics_comp.hpp"


namespace mo {
namespace sys {
namespace graphic {

	using namespace renderer;
	using namespace unit_literals;

	namespace {
		bool is_lazy_particle(Effect_type t) {
			switch(t) {
				case Effect_type::none:              return true;
				case Effect_type::element_fire:      return true;
				case Effect_type::element_frost:     return true;
				case Effect_type::element_water:     return true;
				case Effect_type::element_stone:     return true;
				case Effect_type::element_gas:       return true;
				case Effect_type::element_lightning: return true;
				case Effect_type::health:            return true;


				case Effect_type::flame_thrower:     return false;
				case Effect_type::burning:           return false;
				case Effect_type::poisoned:          return false;
				case Effect_type::frozen:            return false;
				case Effect_type::confused:          return false;

				case Effect_type::explosion_fire:    return true;
				case Effect_type::explosion_poison:  return true;
				case Effect_type::explosion_ice:     return true;
				case Effect_type::explosion_stone:   return true;

				default:
					return false;
			}
			FAIL("UNREACHABLE, maybe");
		}
	}

	Graphic_system::Graphic_system(
	        ecs::Entity_manager& entity_manager,
	        sys::physics::Transform_system& ts,
	        asset::Asset_manager& asset_manager,
	        renderer::Particle_renderer& particle_renderer,
	        state::State_system& state_system) noexcept
		: effects(&Graphic_system::add_effect, this),
	      _assets(asset_manager),
	      _particle_renderer(particle_renderer),
	      _transform(ts),
	      _sprite_batch(asset_manager),
	      _sprites(entity_manager.list<Sprite_comp>()),
	      _particles(entity_manager.list<Particle_emiter_comp>()),
	      _state_change_slot(&Graphic_system::_on_state_change, this)
	{
		_state_change_slot.connect(state_system.state_change_events);

		entity_manager.register_component_type<Sprite_comp>();
		entity_manager.register_component_type<Particle_emiter_comp>();
	}


	void Graphic_system::add_effect(ecs::Entity& entity, Effect_type type) {
		auto emiter_m = entity.get<Particle_emiter_comp>();
		auto& emiter = emiter_m.is_some() ? emiter_m.get_or_throw()
		                                  : entity.emplace<Particle_emiter_comp>();

		for(auto i : util::range(Particle_emiter_comp::max_emiters)) {
			auto& e = emiter._emiters[i];
			if(e._type==type) {
				emiter.particle_type(i, type, false);
				emiter.enabled(i, true, true);
				return;
			}
		}

		for(auto i : util::range(Particle_emiter_comp::max_emiters)) {
			auto& e = emiter._emiters[i];
			if(!e._enabled) {
				emiter.particle_type(i, type, false);
				emiter.enabled(i, true, true);
				return;
			}
		}
	}

	void Graphic_system::draw(const renderer::Camera& camera) noexcept{
		glm::vec2 upper_left = camera.screen_to_world({camera.viewport().x, camera.viewport().y});
		glm::vec2 lower_right = camera.screen_to_world({camera.viewport().z, camera.viewport().w});

		_transform.foreach_in_rect(upper_left, lower_right, [&](ecs::Entity& entity) {
			process(entity.get<physics::Transform_comp>(),
	                entity.get<Sprite_comp>())
            >> [&](const auto& trans, const auto& sp) {
				auto sprite = sp.sprite();
				sprite.position = trans.position();
				sprite.layer = trans.layer();
				sprite.rotation = trans.rotation();
				_sprite_batch.draw(camera, sprite);
	        };
		});

		_sprite_batch.drawAll(camera);
	}

	void Graphic_system::update(Time dt) noexcept{
		for(auto& sprite : _sprites) {
			sprite.current_frame(
				sprite._animation->next_frame(
					sprite.animation_type(), sprite.current_frame(), dt.value(), sprite._repeat_animation
				)
			);
		}

		for(auto& p : _particles) {
			int i = -1;
			for(auto& e : p._emiters) {
				i++;
				if(!e._enabled)
					continue;

				_create_emiter(e);
				if(!e._emiter)
					continue;

				auto transform = p.owner().get<physics::Transform_comp>();
				auto physics = p.owner().get<physics::Physics_comp>();

				transform.process([&](auto& t) {
					auto vel = Velocity{0,0};
					if(!is_lazy_particle(e._type))
						vel = physics.process(vel, [](auto& p){return p.velocity();});

					e._emiter->update_center(t.position(), t.rotation(), vel);
				});

				if(e._scale) {
					physics.process([&](auto& phys){
						p.scale(i, phys.radius());
					});
				}

				if(e._temporary) {
					if(e._to_be_disabled)
						p.enabled(i, false);
					else
						e._to_be_disabled = true;
				}
			}
		}
	}

	namespace {
		Particle_emiter_ptr create_orb_emiter(Texture_ptr tex,
		                                      Particle_renderer& particle_renderer) {
			if(tex) {
				return particle_renderer.create_emiter(
						{0_m,0_m},
						0_deg,
						0.5_m,
						0_m,
						renderer::Collision_handler::none,
						15,
						100,
						0.05_s, 0.2_s,
						util::scerp<Angle>(0_deg, 0_deg),
						util::scerp<Angle>(0_deg, 0_deg),
						util::lerp<Speed_per_time>(8_m/second_2, 0_m/second_2),
						util::lerp<Angle_acceleration>(3000_deg/second_2, 0_deg/second_2),
						util::lerp<glm::vec4>({0.6,0.6,0.6,0.3}, {0,0,0,0.0}),
						util::lerp<Position>({25_cm, 25_cm}, {60_cm, 60_cm}, {2_cm, 2_cm}),
						util::scerp<int8_t>(0),
						tex
				);
			}

			return {};
		}

		Particle_emiter_ptr create_thrower_emiter(Texture_ptr tex,
		                                      Particle_renderer& particle_renderer, float count_fac=1) {
			if(tex) {
				return particle_renderer.create_emiter(
						{0_m,0_m},
						0_deg,
						0.01_m,
						0.55_m,
						renderer::Collision_handler::bounce,
						400 * count_fac,
						500,
						1.0_s, 1.2_s,
						util::scerp<Angle>(0_deg, 15_deg),
						util::scerp<Angle>(0_deg, 0_deg),
						util::lerp<Speed_per_time>(10_m/second_2, 0_m/second_2),
						util::scerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
						util::lerp<glm::vec4>({0.8,0.8,0.8,0}, {0,0,0,0.5}),
						util::lerp<Position>({20_cm, 20_cm}, {100_cm, 100_cm}, {10_cm, 10_cm}),
						util::scerp<int8_t>(0),
						tex
				);
			}

			return {};
		}

		Particle_emiter_ptr create_explosion_emiter(Texture_ptr tex,
		                                      Particle_renderer& particle_renderer, float zoom=1.f) {
			if(tex) {
				return particle_renderer.create_emiter(
						{0_m,0_m},
						0_deg,
						0.1_m,
						0._m,
						renderer::Collision_handler::bounce,
						200 * 60 / zoom,
						600 / zoom,
						0.3_s, 1.0_s,
						util::scerp<Angle>(0_deg, 360_deg),
						util::scerp<Angle>(0_deg, 0_deg),
						util::lerp<Speed_per_time>(8_m/second_2, 2_m/second_2, 2_m/second_2),
						util::scerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
						util::lerp<glm::vec4>({1,1,1,0}, {0,0,0,0}),
						util::lerp<Position>({ 20_cm*zoom,  20_cm*zoom},
						                     {100_cm*zoom, 100_cm*zoom},
						                     { 10_cm*zoom,  10_cm*zoom}),
						util::scerp<int8_t>(0),
						tex
				);
			}

			return {};
		}

		Texture_ptr load_tex(asset::Asset_manager& a, std::string tex_name) {
			return a.load<renderer::Texture>(asset::AID(
					asset::Asset_type::tex, std::move(tex_name)));
		}
	}

	void Graphic_system::_create_emiter(Particle_emiter_comp::Emiter& e) {
		if(!e._emiter && e._enabled) {
			switch(e._type) {
				case Effect_type::none:
					e._enabled = false;
					return;

				case Effect_type::element_fire:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_fire"),
					                              _particle_renderer);
					break;
				case Effect_type::element_frost:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_frost"),
					                              _particle_renderer);
					break;
				case Effect_type::element_water:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_water"),
					                              _particle_renderer);
					break;
				case Effect_type::element_stone:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_stone"),
					                              _particle_renderer);
					break;
				case Effect_type::element_gas:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_gas"),
					                              _particle_renderer);
					break;
				case Effect_type::element_lightning:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_lightning"),
					                              _particle_renderer);
					break;
				case Effect_type::health:
					e._emiter = create_orb_emiter(load_tex(_assets,"particle_health"),
					                              _particle_renderer);
					break;

				case Effect_type::flame_thrower:
					e._emiter = create_thrower_emiter(load_tex(_assets,"particle_fire"),
					                                  _particle_renderer);
					break;

				case Effect_type::poison_thrower:
					e._emiter = create_thrower_emiter(load_tex(_assets,"particle_gas"),
					                                  _particle_renderer);
					break;

				case Effect_type::frost_thrower:
					e._emiter = create_thrower_emiter(load_tex(_assets,"particle_frost"),
					                                  _particle_renderer);
					break;

				case Effect_type::water_thrower:
					e._emiter = create_thrower_emiter(load_tex(_assets,"particle_water"),
					                                  _particle_renderer);
					break;

				case Effect_type::wind_thrower:
					e._emiter = create_thrower_emiter(load_tex(_assets,"particle_wind"),
					                                  _particle_renderer, 0.2f);
					break;


				case Effect_type::steam:
					e._emiter = _particle_renderer.create_emiter(
							{0_m,0_m},
							0_deg,
							0.8_m,
							0_m,
							renderer::Collision_handler::bounce,
							10,
							60,
							0.8_s, 1.0_s,
							util::scerp<Angle>(0_deg, 360_deg),
							util::scerp<Angle>(0_deg, 0_deg),
							util::lerp<Speed_per_time>(4_m/second_2, 2_m/second_2),
							util::lerp<Angle_acceleration>(0_deg/second_2, 10_deg/second_2),
							util::lerp<glm::vec4>({0.8,0.8,0.8,0.8}, {0,0,0,0}),
							util::lerp<Position>({50_cm, 50_cm}, {80_cm, 80_cm}, {20_cm, 20_cm}),
							util::scerp<int8_t>(0),
							load_tex(_assets, "particle_steam")
					);
					break;


				case Effect_type::burning:
					e._emiter = _particle_renderer.create_emiter(
							{0_m,0_m},
							0_deg,
							0.2_m,
							0.0_m,
							renderer::Collision_handler::bounce,
							30,
							100,
							0.8_s, 1.0_s,
							util::scerp<Angle>(0_deg, 360_deg),
							util::scerp<Angle>(0_deg, 0_deg),
							util::lerp<Speed_per_time>(2_m/second_2, 0_m/second_2),
							util::scerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
							util::lerp<glm::vec4>({0.3,0.2,0.2,0}, {0,0,0,0.2}),
							util::lerp<Position>({40_cm, 40_cm}, {80_cm, 80_cm}, {10_cm, 10_cm}),
							util::scerp<int8_t>(0),
							load_tex(_assets,"particle_fire")
					);
					break;

				case Effect_type::poisoned:
					e._emiter = _particle_renderer.create_emiter(
							{0_m,0_m},
							0_deg,
							0.2_m,
							0.0_m,
							renderer::Collision_handler::kill,
							30,
							100,
							0.5_s, 0.8_s,
							util::scerp<Angle>(0_deg, 360_deg),
							util::scerp<Angle>(0_deg, 0_deg),
							util::lerp<Speed_per_time>(2_m/second_2, 0_m/second_2),
							util::scerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
							util::lerp<glm::vec4>({0.5,0.5,0.5,0}, {0,0,0,0}),
							util::lerp<Position>({40_cm, 40_cm}, {80_cm, 80_cm}, {10_cm, 10_cm}),
							util::scerp<int8_t>(0),
							load_tex(_assets,"particle_gas")
					);
					break;

				case Effect_type::frozen:
					e._emiter = _particle_renderer.create_emiter(
							{0_m,0_m},
							0_deg,
							0.2_m,
							0.0_m,
							renderer::Collision_handler::kill,
							30,
							100,
							0.5_s, 0.8_s,
							util::scerp<Angle>(0_deg, 360_deg),
							util::scerp<Angle>(0_deg, 0_deg),
							util::lerp<Speed_per_time>(2_m/second_2, 0_m/second_2),
							util::scerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
							util::lerp<glm::vec4>({0.5,0.5,0.5,0}, {0,0,0,0}),
							util::lerp<Position>({40_cm, 40_cm}, {80_cm, 80_cm}, {10_cm, 10_cm}),
							util::scerp<int8_t>(0),
							load_tex(_assets,"particle_frost")
					);
					break;

				case Effect_type::confused:
					e._emiter = _particle_renderer.create_emiter(
							{0_m,0_m},
							0_deg,
							0.3_m,
							0.0_m,
							renderer::Collision_handler::kill,
							2,
							20,
							0.8_s, 1.0_s,
							util::scerp<Angle>(180_deg, 80_deg),
							util::scerp<Angle>(90_deg, 0_deg),
							util::lerp<Speed_per_time>(4_m/second_2, 1_m/second_2),
							util::scerp<Angle_acceleration>(0_deg/second_2, 5_deg/second_2),
							util::lerp<glm::vec4>({1.0,1.0,1.0,0}, {0,0,0,0}),
							util::lerp<Position>({60_cm, 60_cm}, {80_cm, 80_cm}, {10_cm, 10_cm}),
							util::scerp<int8_t>(0),
							load_tex(_assets,"particle_questionmark")
					);
					break;

				case Effect_type::explosion_fire:
					e._emiter = create_explosion_emiter(load_tex(_assets,"particle_fire"),
					                                    _particle_renderer);
					break;

				case Effect_type::explosion_poison:
					e._emiter = create_explosion_emiter(load_tex(_assets,"particle_gas"),
					                                    _particle_renderer);
					break;

				case Effect_type::explosion_ice:
					e._emiter = create_explosion_emiter(load_tex(_assets,"particle_ice"),
					                                    _particle_renderer, 0.3f);
					break;

				case Effect_type::explosion_stone:
					e._emiter = create_explosion_emiter(load_tex(_assets,"particle_stone_shard"),
					                                    _particle_renderer, 0.3f);
					break;

					// TODO
			}
		}
	}

	void Graphic_system::_on_state_change(ecs::Entity& entity, state::State_data& data){

		entity.get<Sprite_comp>().process([&](Sprite_comp& sprite) {
			bool toRepeat = false;
			renderer::Animation_type type;

			// Map Entity-State to Animation-Type
			switch(data.s){
				case state::Entity_state::idle:
					type = renderer::Animation_type::idle;
					toRepeat = true;
					break;
				case state::Entity_state::walking:
					type = renderer::Animation_type::walking;
					toRepeat = true;
					break;
				case state::Entity_state::attacking_melee:
					type = renderer::Animation_type::attacking_melee;
					toRepeat = false;
					break;
				case state::Entity_state::attacking_range:
					type = renderer::Animation_type::attacking_range;
					toRepeat = true;
					break;
				case state::Entity_state::change_weapon:
					type = renderer::Animation_type::change_weapon;
					toRepeat = false;
					break;
				case state::Entity_state::taking:
					type = renderer::Animation_type::taking;
					toRepeat = false;
					break;
				case state::Entity_state::interacting:
					type = renderer::Animation_type::interacting;
					toRepeat = false;
					break;
				case state::Entity_state::damaged:
					type = renderer::Animation_type::damaged;
					toRepeat = false;
					break;
				case state::Entity_state::healed:
					type = renderer::Animation_type::healed;
					toRepeat = false;
					break;
				case state::Entity_state::dead:
					type = renderer::Animation_type::died;
					toRepeat = true;
					break;
				case state::Entity_state::dying:
					type = renderer::Animation_type::died; // TODO[seb]: dying? last frame from died?
					toRepeat = false;
					break;
				case state::Entity_state::resurrected:
					type = renderer::Animation_type::resurrected;
					toRepeat = false;
					break;
				default:
					type = renderer::Animation_type::idle;
					toRepeat = false;
					break;
			}

			// applying new animation type
			sprite._repeat_animation = toRepeat;
			sprite.animation_type(type);

			// applying magnitude
			sprite.animation()->modulation(type, data.magnitude);

			// calculating remaining time for current animation and inform state_comp about it
			data.min_time(sprite.animation()->remaining_time(sprite.animation_type(), sprite.current_frame()));
		});
	}

}
}
}
