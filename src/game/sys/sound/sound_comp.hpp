/**************************************************************************\
 *	sound_comp.hpp	- Component class for Sounds                          *
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
#include <SDL.h>

namespace mo {
namespace sys {
namespace sound {

	class Sound_comp : public ecs::Component<Sound_comp> {

	public:

		static constexpr const char* name() {return "Sprite";}
		void load(ecs::Entity_state&)override;
		void store(ecs::Entity_state&)override;

		Sound_comp(ecs::Entity& owner) : Component(owner){}

		struct Persisted_state;
		friend struct Persisted_state;

	private:
		friend class Sound_system;

	};

}
}
}
