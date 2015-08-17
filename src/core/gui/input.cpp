#include "input.hpp"


namespace mo {
namespace gui {

	using namespace glm;

	Input::Input(Ui_ctx& ctx, Input_listerner finished, int32_t max, Listerner focused)
	    : Widget(ctx),
	      _border(ctx.create_texture("input")),
	      _text(ctx.create_text("")),
	      _finished(finished), _focused(focused), _max(max) {
	}

	void Input::draw(bool active) {
		auto clip = active
			? vec4{0, 1.f/3, 1, 1.f/3}
			: vec4{0,     0, 1, 1.f/3};

		auto pos = position() + size()/2.f;

		_ctx.draw(_border, pos, active, clip);

		_ctx.draw(_text, pos, active);

		if(active && !_last_active) {
			_ctx.play_focus();

			if(_focused)
				_focused();
		}

		_last_active = active;
	}

	void Input::on_input(const std::string& c) {
		switch(c[0]) {
			case '\r':
				_finished(_text_str);
				break;

			case '\b':
				if(_text_str.empty() )
					return;

				if((_text_str.back()>>7) == 1) { // multi-byte
					do {
						_text_str.pop_back();
					} while(!_text_str.empty() && (_text_str.back()>>6)!=3);

					if(!_text_str.empty())
						_text_str.pop_back();

				} else
					_text_str.pop_back();


				_text.set(_text_str);
				break;

			default:
				if(_max>0 && _text_str.size()<std::size_t(_max)) {
					_text_str.append(c);
					_text.set(_text_str);
				}
		}

		_ctx.play_activate();
	}

	auto Input::size()const noexcept -> glm::vec2 {
		return {_border->width(), _border->height()/3.f};
	}

}
}
