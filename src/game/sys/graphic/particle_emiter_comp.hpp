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
#include <array>

#include "../../effects.hpp"

namespace mo {
namespace sys {
namespace graphic {

	class Particle_emiter_comp : public ecs::Component<Particle_emiter_comp> {
		public:
			static constexpr const char* name() {return "Particle_emiter";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Particle_emiter_comp(ecs::Entity& owner)
			    : Component(owner) {}

			void enabled(std::size_t i, bool, bool temp=false);
			auto enabled(std::size_t i)const noexcept;

			void particle_type(std::size_t i, Effect_type t,
			                   bool scale=false);
			auto particle_type(std::size_t i)const noexcept;

			struct Persisted_state;
			friend struct Persisted_state;
			friend class Graphic_system;

		private:
			static constexpr std::size_t max_emiters = 2;

			struct Emiter {
				renderer::Particle_emiter_ptr _emiter;
				Effect_type                   _type = Effect_type::none;
				bool                          _enabled = false;
				bool                          _to_be_disabled = false;
				bool                          _temporary = false;
				bool                          _scale = false;
			};

			void scale(std::size_t i,Distance r);

			std::array<Emiter, max_emiters> _emiters;
	};

}
}
}

