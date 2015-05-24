#include "sprite_system.hpp"
#include <game/sys/sprite/sprite_comp.hpp>

#include <core/units.hpp>


namespace mo {
namespace sys {
namespace sprite {

	using namespace unit_literals;

	Sprite_system::Sprite_system(ecs::Entity_manager& entity_manager, sys::physics::Transform_system& ts,
								 asset::Asset_manager& asset_manager, state::State_system& state_system) noexcept
		: _transform(ts),
		  _sprite_batch(asset_manager),
		  _sprites(entity_manager.list<Sprite_comp>()),
		  _state_change_slot(&Sprite_system::_on_state_change, this)
	{
		_state_change_slot.connect(state_system.state_change_events);

		entity_manager.register_component_type<Sprite_comp>();
	}

	void Sprite_system::draw(const renderer::Camera& camera) noexcept{
		glm::vec2 upper_left = camera.screen_to_world({camera.viewport().x, camera.viewport().y});
		glm::vec2 lower_right = camera.screen_to_world({camera.viewport().z, camera.viewport().w});

		_transform.foreach_in_rect(upper_left, lower_right, [&](ecs::Entity& entity) {
			process(entity.get<sys::physics::Transform_comp>(),
	                entity.get<sys::sprite::Sprite_comp>())
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

	void Sprite_system::update(Time dt) noexcept{
		for(auto& sprite : _sprites) {

			sprite.current_frame(
				sprite._animation->next_frame(
					sprite.animation_type(), sprite.current_frame(), dt.value(), true
				)
			);

		}
	}

	void Sprite_system::_on_state_change(ecs::Entity& entity, state::State_data& data){

		renderer::Animation_type type;

		/*DEBUG("Entity "
			  << entity.get<Sprite_comp>().get_or_throw()._animation->texture().aid().name()
			  << " -> animation: " << static_cast<int>(data.s)
			  << std::endl);*/

		// Map Entity-State to Animation-Type
		switch(data.s){
			case state::Entity_state::idle:
				type = renderer::Animation_type::idle;
				break;
			case state::Entity_state::walking:
				type = renderer::Animation_type::walking;
				break;
			case state::Entity_state::attacking_melee:
				type = renderer::Animation_type::attacking_melee;
				break;
			case state::Entity_state::attacking_range:
				type = renderer::Animation_type::attacking_range;
				break;
			case state::Entity_state::change_weapon:
				type = renderer::Animation_type::change_weapon;
				break;
			case state::Entity_state::taking:
				type = renderer::Animation_type::taking;
				break;
			case state::Entity_state::interacting:
				type = renderer::Animation_type::interacting;
				break;
			case state::Entity_state::damaged:
				type = renderer::Animation_type::damaged;
				break;
			case state::Entity_state::healed:
				type = renderer::Animation_type::healed;
				break;
			case state::Entity_state::dead:
				type = renderer::Animation_type::died;
				break;
			case state::Entity_state::dying:
				type = renderer::Animation_type::died; // TODO[seb]: dying? last frame from died?
				break;
			case state::Entity_state::resurrected:
				type = renderer::Animation_type::resurrected;
				break;
			default:
				type = renderer::Animation_type::idle;
				break;
		}

		// TODO: use me  data.min_time(1_s);

		// applying new animation type
		entity.get<Sprite_comp>().get_or_throw().animation_type(type);

	}

}
}
}
