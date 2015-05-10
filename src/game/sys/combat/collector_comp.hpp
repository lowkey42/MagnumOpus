/**************************************************************************\
 * FRIENDSHIP IS MAGIC! and protects against friendly fire                *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <core/ecs/ecs.hpp>
#include <core/units.hpp>

namespace mo {
namespace sys {
namespace physics{class Transform_system;}

namespace combat {



	class Collector_comp : public ecs::Component<Collector_comp> {
		public:
			static constexpr const char* name() {return "Collector";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Collector_comp(ecs::Entity& owner, int group=0) noexcept
				: Component(owner), _group(group) {}

			void take(physics::Transform_system& ts);

			struct Persisted_state;
			friend struct Persisted_state;
		private:
			float _group;
	};

}
}
}
