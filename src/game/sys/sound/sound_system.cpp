#include "sound_system.hpp"
#include <game/sys/sound/sound_comp.hpp>

#include <core/units.hpp>


namespace mo {
namespace sys {
namespace sound {

	using namespace unit_literals;

	Sound_system::Sound_system(ecs::Entity_manager& entity_manager, physics::Transform_system& ts,
							   audio::Sound_ctx& snd_ctx, asset::Asset_manager& asset_manager,
							   state::State_system& state_system) noexcept
		: _transform(ts),
		  _sound_ctx(snd_ctx),
		  _sounds(entity_manager.list<Sound_comp>()),
		  _state_change_slot(&Sound_system::_on_state_change, this)
	{
		_state_change_slot.connect(state_system.state_change_events);

		entity_manager.register_component_type<Sound_comp>();
	}


	void Sound_system::play_sounds(const renderer::Camera& camera) noexcept {
		glm::vec2 upper_left = camera.screen_to_world({camera.viewport().x, camera.viewport().y});
		glm::vec2 lower_right = camera.screen_to_world({camera.viewport().z, camera.viewport().w});

		_transform.foreach_in_rect(upper_left, lower_right, [&](ecs::Entity& entity) {
			process(entity.get<sys::physics::Transform_comp>(),
					entity.get<sys::sound::Sound_comp>())
			>> [&](const sys::physics::Transform_comp& trans, sys::sound::Sound_comp& snd) {

				int loop = 0;

				// Moving cam to origin -> move entity the same dist in x and y and calculate angle between axis and entity-pos
				glm::mat4 translation = glm::translate(glm::mat4(), glm::vec3(-camera.position().x, -camera.position().y, 0.f));
				glm::vec4 camPos = translation * glm::vec4(camera.position(), 0.0f, 1.0f);
				glm::vec4 entityRelToCam = translation * glm::vec4(trans.position().x.value(), trans.position().y.value(), 0.f, 1.0f);

				// Calculate distance (r) and angle(alpha)
				Distance dist = Distance(glm::distance(camPos, entityRelToCam) * 16.f);
				float degree = (glm::atan(entityRelToCam.y, entityRelToCam.x) * 180.f / glm::pi<float>()) + 90.f;
				if(degree < 0)
					degree = 360 - std::abs(degree);
				Angle ang = Angle(degree);

				DEBUG("Angle: " << ang.value() << " Distance: " << dist.value());
				snd.channel_id(_sound_ctx.play(snd.get_sound(snd._state), ang, dist, loop));
				//DEBUG("Got something with trans & snd -> ref " << &entity << " with Channel_id: " << snd.channel_id());




			};
		});


	}


	void Sound_system::update(Time dt) noexcept {
		for(auto& sound : _sounds){


		}
	}


	void Sound_system::_on_state_change(ecs::Entity& entity, state::State_data& data){

		// TODO

	}

}
}
}
