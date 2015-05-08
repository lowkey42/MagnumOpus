/**************************************************************************\
 *												                          *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <core/ecs/ecs.hpp>
#include <core/renderer/texture.hpp>
#include <core/renderer/sprite_batch.hpp>
#include <core/renderer/animation.hpp>

namespace mo {
namespace sys {
namespace sprite {

	class Sprite_comp : public ecs::Component<Sprite_comp> {

	public:

		static constexpr const char* name() {return "Sprite";}
		void load(ecs::Entity_state&)override;
		void store(ecs::Entity_state&)override;

		// TODO: nullptr check
		Sprite_comp(ecs::Entity& owner, asset::Ptr<renderer::Animation> animation = asset::Ptr<renderer::Animation>()) :
			Component(owner), _animation(animation), _animType(renderer::Animation_type::idle){}

        auto sprite() const noexcept {
			// Calculating corresponding uv-coords
			// uv-coords -> 1: x = xStart from left | 2: y = yStart from down | 3: z = xEnd from left | 4: w = yEnd from down

			int row = _animation->animations.find(_animType) -> second.row;

			float width = _animation->frame_width / static_cast<float>(_animation->texture->width());
			float height = _animation->frame_height / static_cast<float>(_animation->texture->height());
			float startX = 0.0f;
			float startY = _animation - height - (row * height);
			const glm::vec4 uv = glm::vec4(startX, startY, startX + width, startY + height);
			return renderer::Sprite_batch::Sprite{{}, 0, uv, _animation};
		}

		struct Persisted_state;
		friend struct Persisted_state;

	private:

		asset::Ptr<renderer::Animation> _animation;
		renderer::Animation_type _animType;

	};

}
}
}
