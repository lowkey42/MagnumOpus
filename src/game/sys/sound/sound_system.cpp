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

				if(_sound_ctx.sound_playing(snd.channel_id())){

					// Moving cam to origin -> move entity the same dist in x and y and calculate angle between axis and entity-pos
					glm::mat4 translation = glm::translate(glm::mat4(), glm::vec3(-camera.position().x, -camera.position().y, 0.f));
					glm::vec4 camPos = translation * glm::vec4(camera.position(), 0.0f, 1.0f);
					glm::vec4 entityRelToCam = translation * glm::vec4(trans.position().x.value(), trans.position().y.value(), 0.f, 1.0f);

					DEBUG("UPDATING ANGLE AND DISTANCE");
					// Calculate distance (r) and angle(alpha)
					Distance dist = Distance(glm::distance(camPos, entityRelToCam) * 16.f);
					float degree = (glm::atan(entityRelToCam.y, entityRelToCam.x) * 180.f / glm::pi<float>()) + 90.f;
					if(degree < 0)
						degree = 360 - std::abs(degree);
					Angle ang = Angle(degree);

					_sound_ctx.update_position(snd._assigned_channel, ang, dist);
				}

			};
		});


	}


	void Sound_system::update(Time dt) noexcept {
		for(auto& sound : _sounds){
			if(sound.owner().get<Sound_comp>().is_nothing()){
				DEBUG("Owner not there anymore!");
				_sound_ctx.stop(sound._assigned_channel);
			}
		}
	}


	void Sound_system::_on_state_change(ecs::Entity& entity, state::State_data& data){

		if(entity.get<Sound_comp>().is_some()){

			auto& snd = entity.get<Sound_comp>().get_or_throw();
			snd.get_sound(0);

			int loop = 0;
			using estate = state::Entity_state;

			switch(data.s){

				case(estate::idle):
					loop = 0;
					break;

				case(estate::walking):
					loop = -1;
					break;

				case(estate::attacking_melee):
					loop = 0;
					break;

				case(estate::attacking_range):
					loop = 0;
					break;

				case(estate::interacting):
					loop = 0;
					break;

				case(estate::taking):
					loop = 0;
					break;

				case(estate::change_weapon):
					loop = 0;
					break;

				case(estate::damaged):
					loop = 0;
					break;

				case(estate::healed):
					loop = 0;
					break;

				case(estate::dying):
					loop = -1;
					break;

				case(estate::dead):
					loop = -1;
					break;

				case(estate::resurrected):
					loop = 0;
					break;

			}

			_sound_ctx.stop(snd._assigned_channel);
			snd._loop = loop;
			snd._state = (static_cast<int>(data.s) > 0) ? static_cast<int>(data.s) : 0;

			if(!_sound_ctx.sound_playing(snd.channel_id())){
				snd.channel_id(_sound_ctx.play(snd.get_sound(snd._state), Angle(0), Distance(0), snd._loop));
				DEBUG("Playing Sound at Channel " << snd._assigned_channel);
			}

		}
	}
}
}
}
