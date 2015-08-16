/**************************************************************************\
 * base classes for the simple game ui                                    *
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

#include <vector>
#include <memory>
#include <string>

#include <glm/vec2.hpp>

#include "../renderer/text.hpp"
#include "../renderer/shader.hpp"
#include "../renderer/primitives.hpp"
#include "../audio/audio_ctx.hpp"

namespace mo {
namespace gui {

	using Ui_text   = renderer::Text_dynamic;
	using Ui_texture = renderer::Texture_ptr;

	struct Ui_ctx {
		virtual ~Ui_ctx(){}
		virtual auto create_texture(const std::string& type) -> Ui_texture = 0;
		virtual auto create_text(const std::string& str) -> Ui_text = 0;

		virtual void draw(Ui_texture& tex, glm::vec2 center, bool highlight=false,
			              glm::vec4 clip=glm::vec4{0,0,1,1}) = 0;

		virtual void draw(Ui_text& tex, glm::vec2 center, bool highlight=false) = 0;

		virtual void play_activate() = 0;
		virtual void play_focus() = 0;
	};


	enum class Direction {
		up, down, left, right
	};

	class Widget {
		public:
			Widget(Ui_ctx& ctx, glm::vec2 position=glm::vec2{})
			    : _ctx(ctx), _position(position) {}
			virtual ~Widget(){}

			virtual void draw  (bool active)=0;
			virtual void update(float dt) {}

			virtual void on_activate  () {}
			virtual void on_input     (const std::string& text) {}
			virtual void on_move(glm::vec2 point) {}
			virtual void on_move(Direction dir) {}

			virtual auto size()const noexcept -> glm::vec2 = 0;

			auto position()            const noexcept {return _position;}
			void position(glm::vec2 p)       noexcept;

			auto contains(glm::vec2 p) const noexcept -> bool;

		protected:
			Ui_ctx& _ctx;
			virtual void _on_moved(glm::vec2 p) noexcept {}

		private:
			glm::vec2 _position;
	};

	using Widget_ptr = std::unique_ptr<Widget>;
	using Widget_list = std::vector<Widget_ptr>;

	using Layout = std::function<void(glm::vec2 pos, glm::vec2 size, Widget_list&)>;

	class Widget_container : public Widget {
		public:
			Widget_container(Ui_ctx& ctx, glm::vec2 position, glm::vec2 size,
			                 Layout layout);

			template<class W, class... Args>
			auto add_new(Args&&... args) -> Widget_container& {
				add(std::make_unique<W>(_ctx, std::forward<Args>(args)...));
				return *this;
			}
			auto add(Widget_ptr w) -> Widget_container&;


			void draw(bool active)override;
			void update(float dt)override;

			void on_activate()override;
			void on_input(const std::string& text)override;
			void on_move(glm::vec2 point)override;
			void on_move(Direction dir)override;

			glm::vec2 size()const noexcept override {return _size;}

		protected:
			void _on_moved(glm::vec2 p) noexcept override;
			void _realign();

		private:
			glm::vec2       _size;
			const bool      _auto_size_x;
			const bool      _auto_size_y;
			Layout          _layout;
			int             _focus = 0;
			Widget_list     _children;
	};
	template<class W, class... Args>
	auto operator+=(std::unique_ptr<Widget_container> self, Args&&... args)
			-> std::unique_ptr<Widget_container> {

		self->add(std::forward<Args>(args)...);
		return self;
	}

	extern auto vertical  (float padding) -> Layout;
	extern auto horizontal(float padding) -> Layout;

}
}
