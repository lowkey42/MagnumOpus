#define GLM_SWIZZLE

#include "sound_system.hpp"

#include "../state/state_comp.hpp"

#include <core/units.hpp>


namespace mo {
namespace sys {
namespace sound {

	using namespace unit_literals;

	Sound_system::Sound_system(ecs::Entity_manager& entity_manager, physics::Transform_system& ts,
							   audio::Audio_ctx& audio_ctx) noexcept
		: _transform(ts),
		  _audio_ctx(audio_ctx),
		  _sounds(entity_manager.list<Sound_comp>())
	{
		entity_manager.register_component_type<Sound_comp>();
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

				if(sound) {
					auto p_entity = camera.viewport().zw()/2.f - camera.world_to_screen(remove_units(trans.position()));
					auto angle = Angle(glm::atan(p_entity.y, p_entity.x));
					auto dist = glm::length(p_entity) / max_dist;

					//if(dist<=1.0f) {

						sc._channel = _audio_ctx.play_dynamic(*sound, angle, dist, false, sc._channel);
						return;
					//}
				}

				_audio_ctx.stop(sc._channel);
				sc._channel = -1;
			};
		});


	}
}
}
}
