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

#include "game_engine.hpp"

#include <core/ecs/serializer.hpp>
#include <core/renderer/shader.hpp>
#include <core/renderer/vertex_object.hpp>
#include "sys/state/state_system.hpp"

#include "../core/units.hpp"

#include "game_state.hpp"


namespace mo {
	namespace renderer{ class Camera; }

	struct Game_state;
	struct Saveable_state;

	class Game_screen : public Screen {
		public:
			static bool save_exists(Game_engine& engine);

			Game_screen(Game_engine& engine, Profile_data profile,
			            std::vector<ecs::ETO> players,
			            int depth);

			Game_screen(Game_engine& engine);

			Game_screen(Game_engine& engine, std::string name);

			Game_screen(Game_engine& engine, const Saveable_state& state);
			~Game_screen()noexcept;

			auto save() -> Saveable_state;

		protected:
			Game_screen(Game_engine& engine, std::unique_ptr<Game_state> state);

			void _update(float delta_time)override;
			void _draw(float delta_time)override;

			auto _prev_screen_policy()const noexcept -> Prev_screen_policy override {
				return Prev_screen_policy::discard;
			}

			void _on_state_change(ecs::Entity&, sys::state::State_data&);

			void _on_enter(util::maybe<Screen&> prev) override;
			void _on_leave(util::maybe<Screen&> next) override;

			void _join(sys::controller::Controller_added_event e);
			void _unjoin(sys::controller::Controller_removed_event e);

		private:
			Game_engine& _engine;

			std::unique_ptr<Game_state> _state;

			util::slot<ecs::Entity&, sys::state::State_data&> _player_sc_slot;
			util::slot<sys::controller::Controller_added_event> _join_slot;
			util::slot<sys::controller::Controller_removed_event> _unjoin_slot;

			renderer::Shader_program _post_effects;
			renderer::Shader_program _lightmap_filter;
			renderer::Shader_program _blur_filter;
			renderer::Object _post_effect_obj;
			renderer::Framebuffer _lightmap[2];

			Time _fadein_left = Time{0};
			bool _moving_down = false, _dying = false;
	};

}
