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
		glm::vec2 upper_left = camera.screen_to_world({camera.viewport().x, camera.viewport().y});
		glm::vec2 lower_right = camera.screen_to_world({camera.viewport().z, camera.viewport().w});

		_transform.foreach_in_rect(upper_left, lower_right, [&](ecs::Entity& entity) {
			process(entity.get<sys::physics::Transform_comp>(),
			        entity.get<sys::sound::Sound_comp>(),
			        entity.get<sys::state::State_comp>())
			>> [&](const physics::Transform_comp& trans, Sound_comp& sc, state::State_comp& state) {

				auto sound = determine_sound(sc, state);

				if(!sound) {
					_audio_ctx.stop(sc._channel);
					sc._channel = -1;

				} else {
					// Moving cam to origin -> move entity the same dist in x and y and calculate angle between axis and entity-pos
					glm::mat4 translation = glm::translate(glm::mat4(), glm::vec3(-camera.position().x, -camera.position().y, 0.f));
					glm::vec4 camPos = translation * glm::vec4(camera.position(), 0.0f, 1.0f);
					glm::vec4 entityRelToCam = translation * glm::vec4(trans.position().x.value(), trans.position().y.value(), 0.f, 1.0f);

					// Calculate distance (r) and angle(alpha)
					Distance dist = Distance(glm::distance(camPos, entityRelToCam) * 16.f);
					float degree = (glm::atan(entityRelToCam.y, entityRelToCam.x) * 180.f / glm::pi<float>()) + 90.f;
					if(degree < 0)
						degree = 360 - std::abs(degree); // FIXME[seb]: 360?!
					Angle ang = Angle(degree);

					sc._channel = _audio_ctx.play_dynamic(*sound, ang, dist, false, sc._channel);
				}
			};
		});


	}
}
}
}
