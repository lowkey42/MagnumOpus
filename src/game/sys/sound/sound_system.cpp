#define GLM_SWIZZLE
#define MO_BUILD_SERIALIZER

#include "sound_system.hpp"

#include "../state/state_comp.hpp"

#include <core/units.hpp>

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>


namespace mo {
namespace sys {
namespace sound {

	using namespace unit_literals;

	namespace {
		auto effect_time(Effect_type e) -> Time {
			switch(e) {
				case Effect_type::none:              return 1_s;
				case Effect_type::element_fire:      return 0.1_s;
				case Effect_type::element_frost:     return 0.1_s;
				case Effect_type::element_water:     return 0.1_s;
				case Effect_type::element_stone:     return 0.1_s;
				case Effect_type::element_gas:       return 0.1_s;
				case Effect_type::element_lightning: return 0.1_s;
				case Effect_type::health:            return 0.1_s;
				case Effect_type::blood:             return 0.1_s;

				case Effect_type::flame_thrower:     return 0.6_s;
				case Effect_type::flame_thrower_big: return 0.6_s;
				case Effect_type::poison_thrower:    return 0.6_s;
				case Effect_type::frost_thrower:     return 0.6_s;
				case Effect_type::water_thrower:     return 0.6_s;
				case Effect_type::wind_thrower:      return 0.6_s;

				case Effect_type::steam:             return 1.0_s;
				case Effect_type::poison_cloud:      return 1.0_s;

				case Effect_type::burning:           return 0.6_s;
				case Effect_type::poisoned:          return 0.6_s;
				case Effect_type::frozen:            return 0.6_s;
				case Effect_type::confused:          return 0.6_s;

				case Effect_type::explosion_fire:    return 0.6_s;
				case Effect_type::explosion_poison:  return 0.6_s;
				case Effect_type::explosion_ice:     return 0.6_s;
				case Effect_type::explosion_stone:   return 0.6_s;
			}
			FAIL("UNREACHABLE, maybe");
		}

		struct Sound_effect_data_value {
			std::string sound;
			bool is_static = false;
		};

		struct Sound_effect_data {
			std::unordered_map<Effect_type, Sound_effect_data_value> effects;
		};

		sf2_structDef(Sound_effect_data_value, sf2_member(sound), sf2_member(is_static))
		sf2_structDef(Sound_effect_data, sf2_member(effects))
	}
}
}
}

namespace mo {
namespace asset {
	template<>
	struct Loader<sys::sound::Sound_effect_data> {
		using RT = std::shared_ptr<sys::sound::Sound_effect_data>;

		static RT load(istream in) throw(Loading_failed) {
			auto r = std::make_shared<sys::sound::Sound_effect_data>();

			sf2::parseStream(in, *r);

			return r;
		}

		static void store(ostream out, const sys::sound::Sound_effect_data& asset) throw(Loading_failed) {
			sf2::writeStream(out,asset);
		}
	};
}
}


namespace mo {
namespace sys {
namespace sound {

	Sound_system::Sound_system(asset::Asset_manager& assets,
	                           ecs::Entity_manager& entity_manager,
	                           physics::Transform_system& ts,
							   audio::Audio_ctx& audio_ctx) noexcept
		: effects(&Sound_system::add_effect, this),
	      _transform(ts),
		  _audio_ctx(audio_ctx),
		  _sounds(entity_manager.list<Sound_comp>()),
	      _sound_effects(effect_type_count)
	{
		entity_manager.register_component_type<Sound_comp>();

		auto se_data = assets.load<Sound_effect_data>("cfg:sound_effects"_aid);

		for(std::size_t i=0; i<effect_type_count; ++i) {
			auto iter = se_data->effects.find(static_cast<Effect_type>(i));
			if(iter!=se_data->effects.end()) {
				_sound_effects[i].sound = assets.load<audio::Sound>(asset::AID(iter->second.sound));
				_sound_effects[i].static_sound = iter->second.is_static;
			}
		}
	}

	namespace {
		auto determine_sound(Sound_comp& sc, state::State_comp& state) {
			return sc.get_sound(static_cast<int>(state.state().s));
		}
	}

	void Sound_system::play_sounds(const renderer::Camera& camera) noexcept {
		auto cam_area     = camera.area();
		auto top_left     = cam_area.xy();
		auto bottom_right = cam_area.zw();
		auto max_dist     = glm::length(camera.viewport().zw()) / 2.f;

		_transform.foreach_in_rect(top_left, bottom_right, [&](ecs::Entity& entity) {
			process(entity.get<sys::physics::Transform_comp>(),
			        entity.get<sys::sound::Sound_comp>(),
			        entity.get<sys::state::State_comp>())
			>> [&](const physics::Transform_comp& trans, Sound_comp& sc, state::State_comp& state) {

				auto sound = determine_sound(sc, state);

				if(sound || sc._effect) {
					auto p_entity = camera.viewport().zw()/2.f - camera.world_to_screen(remove_units(trans.position()));
					auto angle = Angle(glm::atan(p_entity.y, p_entity.x));
					auto dist = glm::length(p_entity) / max_dist;

					if(dist<=1.0f) {
						if(sound)
							sc._channel = _audio_ctx.play_dynamic(*sound, angle,
							                                      dist, false, sc._channel);

						if(sc._effect)
							sc._effect_channel = _audio_ctx.play_dynamic(*sc._effect, angle,
							                                             dist, false, sc._effect_channel);
						return;
					}
				}

				if(!sound && sc._channel!=-1) {
					_audio_ctx.stop(sc._channel);
					sc._channel = -1;
				}

				if(!sc._effect && sc._effect_channel!=-1) {
					_audio_ctx.stop(sc._effect_channel);
					sc._effect_channel = -1;
				}

			};
		});

	}

	void Sound_system::update(Time dt) noexcept {
		for(auto& sound : _sounds) {
			if(sound._effect) {
				sound._effect_left -= dt;

				if(sound._effect_left<=0_s) {
					sound._effect.reset();
				}
			}
		}
	}


	void Sound_system::add_effect(ecs::Entity& e, Effect_type effect) {
		auto idx = static_cast<std::size_t>(effect);

		INVARIANT(idx<_sound_effects.size(), "Effect_type is too big");

		auto& sound_effect = _sound_effects[idx];
		if(sound_effect.sound) {
			if(sound_effect.static_sound) {
				_audio_ctx.play_static(*sound_effect.sound);

			} else {
				e.get<sys::sound::Sound_comp>().process([&](auto& sound) {
					sound._effect = sound_effect.sound;
					sound._effect_left = effect_time(effect);
				});
			}
		}
	}

}
}
}
