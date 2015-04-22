#include "sprite_system.hpp"
#include <game/sys/sprite/sprite_comp.hpp>

#include <core/units.hpp>


namespace game{
namespace sys{
namespace sprite{

	Sprite_system::Sprite_system(core::ecs::Entity_manager& entity_manager, game::sys::physics::Transform_system& ts,
	                             core::asset::Asset_manager& asset_manager) noexcept :
	    _transform(ts), _sprite_batch(asset_manager) {
		;
	}

	void Sprite_system::draw(const core::renderer::Camera& camera) noexcept{
		glm::vec2 upper_left = camera.screen_to_world({camera.viewport().x, camera.viewport().y});
		glm::vec2 lower_right = camera.screen_to_world({camera.viewport().z, camera.viewport().w});

		_transform.foreach_in_rect(upper_left, lower_right, [&](core::ecs::Entity& entity) {
			process(entity.get<sys::physics::Transform_comp>(),
	                entity.get<sys::sprite::Sprite_comp>())
	        >> [&](const auto& trans, const auto& sp) {
				struct core::renderer::Sprite_batch::Sprite sprite;
				sprite.position = trans.position();
				sprite.rotation = trans.rotation();
				sprite.texture = sp.sprite().texture;
				sprite.uv = sp.sprite().uv;
				_sprite_batch.draw(camera, sprite);
	        };
		});

		_sprite_batch.drawAll(camera);

	}

	void Sprite_system::update(core::Time dt) noexcept{
		;
	}


}
}
}
