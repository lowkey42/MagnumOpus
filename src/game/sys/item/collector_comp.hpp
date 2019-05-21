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
namespace renderer {class Particle_emiter;}

namespace sys {
namespace physics{class Transform_system;}

namespace item {

	class Collector_comp : public ecs::Component<Collector_comp> {
		public:
			static constexpr const char* name() {return "Collector";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Collector_comp(ecs::Entity& owner) noexcept : Component(owner) {}

			void take()noexcept {_active=true;}

			struct Persisted_state;
			friend struct Persisted_state;
			friend class Item_system;
		private:
			Force    _force{500};
			Distance _near{0.5};
			Distance _far{10};
			Angle    _near_angle{PI};
			Angle    _far_angle{PI};

			bool _active=false;
			std::shared_ptr<renderer::Particle_emiter> _particles;
	};

}
}
}
