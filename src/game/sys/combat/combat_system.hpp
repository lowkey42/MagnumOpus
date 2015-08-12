/**************************************************************************\
 * universal health-care & murder system                                  *
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
#include <core/renderer/primitives.hpp>
#include <core/renderer/camera.hpp>

#include "../physics/transform_system.hpp"
#include "../physics/physics_system.hpp"

#include "../state/state_comp.hpp"
#include "reaper_subsystem.hpp"

#include "comp/weapon_comp.hpp"
#include "comp/health_comp.hpp"
#include "comp/explosive_comp.hpp"
#include "comp/damage_effect_comp.hpp"

#include "../../effects.hpp"

namespace mo {
	namespace asset {class Asset_manager;}
	namespace renderer {class Particle_renderer;}

namespace sys {
namespace combat {

	struct Dmg_effect_data;

	class Combat_system : public Weapon_modifier_collection {
		public:
			Combat_system(asset::Asset_manager& assets,
			              ecs::Entity_manager& entity_manager,
			              physics::Transform_system& transform_system,
			              physics::Physics_system& physics_system,
			              state::State_system& state_system,
			              Effect_source& effects,
			              FFeedback_source& ffeedback);

			void update(Time dt);
			void draw(const renderer::Camera& cam);

		private:
			void _health_care(Time dt);
			void _shoot_something(Time dt);
			void _explode_explosives(Time dt);
			void _deal_ot_effects(Time dt);
			bool _deal_damage(ecs::Entity& target, int group, float damage,
			                  level::Element type = level::Element::neutral,
			                  Damage_effect dmge = Damage_effect::none);
			void _explode(Explosive_comp& e);
			void _draw_ray(Laser_sight_comp& l);

			void _on_collision(physics::Manifold& m);

			ecs::Entity_manager&           _em;
			Weapon_comp::Pool&             _weapons;
			Health_comp::Pool&             _healths;
			Explosive_comp::Pool&          _explosives;
			Laser_sight_comp::Pool&        _lsights;
			Damage_effect_comp::Pool&      _dmg_effects;
			physics::Transform_system&     _ts;
			util::slot<physics::Manifold&> _collision_slot;
			Reaper_subsystem               _reaper;
			renderer::Ray_renderer         _ray_renderer;
			Effect_source&                 _effects;
			FFeedback_source&              _ffeedback;

			std::shared_ptr<const Dmg_effect_data> _dmg_effect_data;
	};

}
}
}
