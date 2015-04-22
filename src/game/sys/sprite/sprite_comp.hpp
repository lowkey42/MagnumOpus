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

namespace game {
namespace sys {
namespace sprite {

	class Sprite_comp : public core::ecs::Component<Sprite_comp> {

	public:

		static constexpr const char* name() {return "Sprite";}
		void load(core::ecs::Entity_state&)override;
		void store(core::ecs::Entity_state&)override;

		Sprite_comp(core::ecs::Entity& owner, std::string aid = "", glm::vec4 uv = glm::vec4(0.0f)) :
		    Component(owner), _texture(aid), _uv(uv){}

		auto sprite() const noexcept {
			struct core::renderer::Sprite_batch::Sprite sprite;
			sprite.texture = _texture;
			sprite.uv = _uv;
			return sprite;
		}

		struct Persisted_state;
		friend struct Persisted_state;

	private:

		core::asset::AID _texture;
		glm::vec4 _uv;

	};

}
}
}
