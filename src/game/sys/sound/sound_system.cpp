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
			>> [&](const sys::physics::Transform_comp& trans, const sys::sound::Sound_comp& snd) {
				DEBUG("Got something with trans & snd with Channel_id: " << snd.Channel_id());
			};
		});


	}


	void Sound_system::update(Time dt) noexcept {
		for(auto& sound : _sounds){


		}
	}


	void Sound_system::_on_state_change(ecs::Entity& entity, state::State_data& data){
		// DEBUG("Executed Sound_system::_on_state_change");
		// TODO
	}

}
}
}
