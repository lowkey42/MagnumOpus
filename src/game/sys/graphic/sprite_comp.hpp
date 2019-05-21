/**************************************************************************\
 *	sprite_comp.hpp	- Component class for Sprites                         *
 *                                                ___                     *
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
namespace graphic {

	class Sprite_comp : public ecs::Component<Sprite_comp> {
		public:
			static constexpr const char* name() {return "Sprite";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Sprite_comp(ecs::Entity& owner, asset::Ptr<renderer::Animation> animation = asset::Ptr<renderer::Animation>()) :
				Component(owner), _animation(animation) {}

			auto sprite() const noexcept -> renderer::Sprite_batch::Sprite;
			auto animation() const noexcept { return _animation; }
			auto current_frame() const noexcept { return _current_frame; }
			auto animation_type() const noexcept { return _anim_type; }
			auto repeat_animation() const noexcept { return _repeat_animation; }
			auto repeat_animation(const bool r) noexcept { _repeat_animation = r; }

			void current_frame(const float frame) noexcept { _current_frame = frame; }
			void animation_type(renderer::Animation_type type) noexcept;

		private:
			friend class Graphic_system;

			float _current_frame = 0;
			bool  _repeat_animation = true;

			asset::Ptr<renderer::Animation> _animation;
			renderer::Animation_type        _anim_type = renderer::Animation_type::idle;

	};

}
}
}
