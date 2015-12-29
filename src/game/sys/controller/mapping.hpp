/**************************************************************************\
 * Mapping from input-signals to commands                                 *
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

#include "../../../core/asset/asset_manager.hpp"

#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <sf2/sf2.hpp>

namespace mo {
namespace sys {
namespace controller {

	enum class Command {
		move_up, move_down, move_left, move_right, attack, use_or_take,
		weapon_0, weapon_1, weapon_2, weapon_3,
		enter_leave_game, quit
	};
	constexpr std::size_t command_count = static_cast<std::size_t>(Command::quit)+1;

	enum class Key : SDL_Keycode {
		invalid       = -1,
		Return        = SDLK_RETURN,
		Escape        = SDLK_ESCAPE,
		Backspace     = SDLK_BACKSPACE,
		Tab           = SDLK_TAB,
		Space         = SDLK_SPACE,
		Exclaim       = SDLK_EXCLAIM,
		Quotedbl      = SDLK_QUOTEDBL,
		Hash          = SDLK_HASH,
		Percent       = SDLK_PERCENT,
		Dollar        = SDLK_DOLLAR,
		Ampersand     = SDLK_AMPERSAND,
		Quote         = SDLK_QUOTE,
		LeftParan     = SDLK_LEFTPAREN,
		RightParan    = SDLK_RIGHTPAREN,
		Asterisk      = SDLK_ASTERISK,
		Plus          = SDLK_PLUS,
		Comma         = SDLK_COMMA,
		Minus         = SDLK_MINUS,
		Period        = SDLK_PERIOD,
		Slash         = SDLK_SLASH,
		T_0           = SDLK_0,
		T_1           = SDLK_1,
		T_2           = SDLK_2,
		T_3           = SDLK_3,
		T_4           = SDLK_4,
		T_5           = SDLK_5,
		T_6           = SDLK_6,
		T_7           = SDLK_7,
		T_8           = SDLK_8,
		T_9           = SDLK_9,
		Colon         = SDLK_COLON,
		Semicolon     = SDLK_SEMICOLON,
		Less          = SDLK_LESS,
		Equals        = SDLK_EQUALS,
		Greater       = SDLK_GREATER,
		Question      = SDLK_QUESTION,
		At            = SDLK_AT,
		LeftBracket   = SDLK_LEFTBRACKET,
		RightBracket  = SDLK_BACKSLASH,
		Backslash     = SDLK_RIGHTBRACKET,
		Caret         = SDLK_CARET,
		Underscore    = SDLK_UNDERSCORE,
		Backquote     = SDLK_BACKQUOTE,
		A             = SDLK_a,
		B             = SDLK_b,
		C             = SDLK_c,
		D             = SDLK_d,
		E             = SDLK_e,
		F             = SDLK_f,
		G             = SDLK_g,
		H             = SDLK_h,
		I             = SDLK_i,
		J             = SDLK_j,
		K             = SDLK_k,
		L             = SDLK_l,
		M             = SDLK_m,
		N             = SDLK_n,
		O             = SDLK_o,
		P             = SDLK_p,
		Q             = SDLK_q,
		R             = SDLK_r,
		S             = SDLK_s,
		T             = SDLK_t,
		U             = SDLK_u,
		V             = SDLK_v,
		W             = SDLK_w,
		X             = SDLK_x,
		Y             = SDLK_y,
		Z             = SDLK_z,
		Capslock      = SDLK_CAPSLOCK,

		F1            = SDLK_F1,
		F2            = SDLK_F2,
		F3            = SDLK_F3,
		F4            = SDLK_F4,
		F5            = SDLK_F5,
		F6            = SDLK_F6,
		F7            = SDLK_F7,
		F8            = SDLK_F8,
		F9            = SDLK_F9,
		F10           = SDLK_F10,
		F11           = SDLK_F11,
		F12           = SDLK_F12,
		PrintScreen   = SDLK_PRINTSCREEN,
		Scrolllock    = SDLK_SCROLLLOCK,
		Pause         = SDLK_PAUSE,

		Left          = SDLK_LEFT,
		Right         = SDLK_RIGHT,
		Up            = SDLK_UP,
		Down          = SDLK_DOWN,

		KP_0          = SDLK_KP_0,
		KP_1          = SDLK_KP_1,
		KP_2          = SDLK_KP_2,
		KP_3          = SDLK_KP_3,
		KP_4          = SDLK_KP_4,
		KP_5          = SDLK_KP_5,
		KP_6          = SDLK_KP_6,
		KP_7          = SDLK_KP_7,
		KP_8          = SDLK_KP_8,
		KP_9          = SDLK_KP_9,

		LControl      = SDLK_LCTRL,
		LShift        = SDLK_LSHIFT,
		LAlt          = SDLK_LALT,
		LSuper        = SDLK_LGUI,
		RControl      = SDLK_RCTRL,
		RShift        = SDLK_RSHIFT,
		RAlt          = SDLK_RALT,
		RSuper        = SDLK_RGUI
	};

	enum class Pad_stick : int8_t {
		invalid       = -1,
		left, right
	};

	enum class Pad_button : int8_t {
		invalid       = -1,

		a              = SDL_CONTROLLER_BUTTON_A,
		b              = SDL_CONTROLLER_BUTTON_B,
		x              = SDL_CONTROLLER_BUTTON_X,
		y              = SDL_CONTROLLER_BUTTON_Y,
		back           = SDL_CONTROLLER_BUTTON_BACK,
		guide          = SDL_CONTROLLER_BUTTON_GUIDE,
		start          = SDL_CONTROLLER_BUTTON_START,
		left_stick     = SDL_CONTROLLER_BUTTON_LEFTSTICK,
		right_stick    = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
		left_shoulder  = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
		right_shoulder = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
		d_pad_up       = SDL_CONTROLLER_BUTTON_DPAD_UP,
		d_pad_down     = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
		d_pad_left     = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
		d_pad_right    = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,

		left_trigger   = SDL_CONTROLLER_AXIS_MAX+1,
		right_trigger

	};

}
}
}

namespace std {
	template <> struct hash<mo::sys::controller::Key> {
		size_t operator()(mo::sys::controller::Key ac)const noexcept {
			return static_cast<size_t>(ac);
		}
	};
	template <> struct hash<mo::sys::controller::Command> {
		size_t operator()(mo::sys::controller::Command ac)const noexcept {
			return static_cast<size_t>(ac);
		}
	};
	template <> struct hash<mo::sys::controller::Pad_button> {
		size_t operator()(mo::sys::controller::Pad_button ac)const noexcept {
			return static_cast<size_t>(ac);
		}
	};
	template <> struct hash<mo::sys::controller::Pad_stick> {
		size_t operator()(mo::sys::controller::Pad_stick ac)const noexcept {
			return static_cast<size_t>(ac);
		}
	};
}

namespace mo {
namespace sys {
namespace controller {

	inline auto to_sdl_keycode(Key key) {
		return static_cast<SDL_Keycode>(key);
	}
	inline auto from_sdl_keycode(SDL_Keycode key) {
		return static_cast<Key>(key);
	}

	inline auto to_sdl_pad_button(Pad_button b) {
		return static_cast<SDL_GameControllerButton>(b);
	}
	inline bool is_trigger(Pad_button b) {
		return b==Pad_button::left_trigger || b==Pad_button::right_trigger;
	}

	inline Pad_button from_sdl_pad_button(SDL_GameControllerButton b) {
		return static_cast<Pad_button>(b);
	}

	inline auto to_sdl_axis_x(Pad_stick s) {
		switch(s) {
			case Pad_stick::left:     return SDL_CONTROLLER_AXIS_LEFTX;
			case Pad_stick::right:    return SDL_CONTROLLER_AXIS_RIGHTX;
			default:                  return SDL_CONTROLLER_AXIS_INVALID;
		}
	}
	inline auto to_sdl_axis_y(Pad_stick s) {
		switch(s) {
			case Pad_stick::left:     return SDL_CONTROLLER_AXIS_LEFTY;
			case Pad_stick::right:    return SDL_CONTROLLER_AXIS_RIGHTY;
			default:                  return SDL_CONTROLLER_AXIS_INVALID;
		}
	}
	inline auto from_sdl_axis(SDL_GameControllerAxis a) {
		switch(a) {
			case SDL_CONTROLLER_AXIS_LEFTX:
			case SDL_CONTROLLER_AXIS_LEFTY:     return Pad_stick::left;
			case SDL_CONTROLLER_AXIS_RIGHTX:
			case SDL_CONTROLLER_AXIS_RIGHTY:    return Pad_stick::right;
			default:                            return Pad_stick::invalid;
		}
	}

	struct Mapping {
		bool mouse_look = true;
		std::unordered_map<Key, Command> key_action;
		std::unordered_map<Uint8, Command> mouse_button_action;
		std::unordered_map<Pad_button, Command> pad_button_action;
		std::vector<Pad_stick> aim_sticks;
		std::vector<Pad_stick> move_sticks;
		float stick_dead_zone;
		float stick_max_value = 32767;
	};
	using Mapping_ptr = std::shared_ptr<const Mapping>;



	sf2_enumDef(Command,
		move_up,
		move_down,
		move_left,
		move_right,
		attack,
		use_or_take,
		weapon_0,
		weapon_1,
		weapon_2,
		weapon_3,
		enter_leave_game,
		quit
	)

	sf2_enumDef(Key,
		Return,
		Escape,
		Backspace,
		Tab,
		Space,
		Exclaim,
		Quotedbl,
		Hash,
		Percent,
		Dollar,
		Ampersand,
		Quote,
		LeftParan,
		RightParan,
		Asterisk,
		Plus,
		Comma,
		Minus,
		Period,
		Slash,
		T_0,
		T_1,
		T_2,
		T_3,
		T_4,
		T_5,
		T_6,
		T_7,
		T_8,
		T_9,
		Colon,
		Semicolon,
		Less,
		Equals,
		Greater,
		Question,
		At,
		LeftBracket,
		RightBracket,
		Backslash,
		Caret,
		Underscore,
		Backquote,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Capslock,

		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		PrintScreen,
		Scrolllock,
		Pause,

		Left,
		Right,
		Up,
		Down,

		KP_0,
		KP_1,
		KP_2,
		KP_3,
		KP_4,
		KP_5,
		KP_6,
		KP_7,
		KP_8,
		KP_9,

		LControl,
		LShift,
		LAlt,
		LSuper,
		RControl,
		RShift,
		RAlt,
		RSuper
	)

	sf2_enumDef(Pad_stick,
		left,
		right
	)

	sf2_enumDef(Pad_button,
		a,
		b,
		x,
		y,
		back,
		guide,
		start,
		left_stick,
		right_stick,
		left_shoulder,
		right_shoulder,
		d_pad_up,
		d_pad_down,
		d_pad_left,
		d_pad_right,
		left_trigger,
		right_trigger
	)

	sf2_structDef(Mapping,
		mouse_look,
		key_action,
		mouse_button_action,
		pad_button_action,
		aim_sticks,
		move_sticks,
		stick_dead_zone
	)
}
}
}
