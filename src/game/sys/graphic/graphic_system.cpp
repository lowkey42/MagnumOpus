#include "graphic_system.hpp"

#include <core/units.hpp>


namespace mo {
namespace sys {
namespace graphic {

	using namespace unit_literals;

	Graphic_system::Graphic_system(ecs::Entity_manager& entity_manager, sys::physics::Transform_system& ts,
								 asset::Asset_manager& asset_manager, state::State_system& state_system) noexcept
		: _transform(ts),
		  _sprite_batch(asset_manager),
		  _sprites(entity_manager.list<Sprite_comp>()),
		  _state_change_slot(&Graphic_system::_on_state_change, this)
	{
		_state_change_slot.connect(state_system.state_change_events);

		entity_manager.register_component_type<Sprite_comp>();
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
	}

	void Graphic_system::_on_state_change(ecs::Entity& entity, state::State_data& data){

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
				toRepeat = false;
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
		Sprite_comp& sprite = entity.get<Sprite_comp>().get_or_throw();
		sprite._repeat_animation = toRepeat;
		sprite.animation_type(type);

		// applying magnitude
		sprite.animation()->modulation(type, data.magnitude);

		// calculating remaining time for current animation and inform state_comp about it
		data.min_time(sprite.animation()->remaining_time(sprite.animation_type(), sprite.current_frame()));
	}

}
}
}
