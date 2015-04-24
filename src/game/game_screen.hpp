/**************************************************************************\
 * The screen in which the core-gameplay take place                       *
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

#include "../game_engine.hpp"

#include "../core/units.hpp"
#include "../core/ecs/ecs.hpp"
#include "../core/ecs/serializer.hpp"

#include "level/level.hpp"
#include "level/tilemap.hpp"

#include "../core/renderer/camera.hpp"

#include "sys/physics/transform_system.hpp"
#include "sys/physics/physics_system.hpp"
#include "sys/sprite/sprite_system.hpp"
#include "sys/controller/controller_system.hpp"


namespace game {
	class Game_master;

	struct Meta_system {
		core::ecs::Entity_manager em;
		core::ecs::Serializer entity_store;

		sys::physics::Transform_system transform;
		sys::physics::Physics_system physics;
		sys::sprite::Sprite_system spritesys;
		sys::controller::Controller_system controller;

		Meta_system(core::Engine& engine, level::Level& level);

		void update(core::Time dt);
		void draw(const core::renderer::Camera& cam);
	};

	class Game_screen : public core::Screen {
		public:
			Game_screen(Game_engine& engine);
			~Game_screen()noexcept;

		protected:
			void _update(float delta_time)override;
			void _draw(float delta_time)override;

			auto _prev_screen_policy()const noexcept -> core::Prev_screen_policy override {
				return core::Prev_screen_policy::discard;
			}

			auto _add_player(sys::controller::Controller& controller) -> core::ecs::Entity_ptr;
			void _join(sys::controller::Controller_added_event e);
			void _unjoin(sys::controller::Controller_removed_event e);

			void _save()const;

		private:
			Game_engine& _engine;

			std::unique_ptr<Game_master> _gm;
			Meta_system _state;

			core::renderer::Camera _camera;
			level::Tilemap _tilemap;

			core::ecs::Entity_ptr _main_player;
			std::vector<core::ecs::Entity_ptr> _sec_players;
	};

}
