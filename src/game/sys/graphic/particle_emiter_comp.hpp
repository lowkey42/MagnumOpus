/**************************************************************************\
 *	wraps a particle emiter                                               *
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
#include <core/renderer/particles.hpp>


namespace mo {
namespace sys {
namespace graphic {

	enum class Particle_emiter_type {
		element_fire,
		element_frost,
		element_water,
		element_stone,
		element_gas,
		element_lightning,

		health
	};

	class Particle_emiter_comp : public ecs::Component<Particle_emiter_comp> {
		public:
			static constexpr const char* name() {return "Particle_emiter";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Particle_emiter_comp(ecs::Entity& owner, bool enabled=true,
			                     Particle_emiter_type type=Particle_emiter_type::element_fire)
			    : Component(owner), _type(type), _enabled(enabled) {}

			void enabled(bool);
			auto enabled()const noexcept {return _enabled;}

			void particle_type(Particle_emiter_type t);
			auto particle_type()const noexcept {return _type;}

			void scale(Distance r);

			struct Persisted_state;
			friend struct Persisted_state;
			friend class Graphic_system;
		private:
			void _create_emiter(renderer::Particle_renderer&, asset::Asset_manager&);

			renderer::Particle_emiter_ptr _emiter;
			Particle_emiter_type          _type;
			bool                          _enabled;
	};

}
}
}

