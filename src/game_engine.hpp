/**************************************************************************\
 * Game-specific engine                                                   *
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

#include "core/engine.hpp"
#include "core/configuration.hpp"

#include "game/sys/controller/controller_system.hpp"


class Game_engine : public core::Engine {
	public:
		Game_engine(const std::string& title, core::Configuration cfg)
		    : Engine(title, std::move(cfg)),
		      _controllers(assets(), input()) {
		}

		/// re-define enter_screen to inject Game_engine instead of Engine into screens
		using core::Engine::enter_screen;

		template<class T, typename ...Args>
		auto enter_screen(Args&&... args) -> T& {
			return static_cast<T&>(enter_screen(std::make_unique<T>(*this, std::forward(args)...)));
		}

		auto controllers()noexcept -> game::sys::controller::Controller_manager & {
			return _controllers;
		}
		auto controllers()const noexcept -> const game::sys::controller::Controller_manager & {
			return _controllers;
		}

	protected:
		void _on_frame(float dt) override {
			_controllers.update(core::Time(dt));
		}

	private:
		game::sys::controller::Controller_manager _controllers;
};
