#include "button.hpp"


namespace mo {
namespace gui {

	using namespace glm;

	Button::Button(Ui_ctx& ctx, std::string label,
	       Listerner clicked, Listerner focused)
	    : Widget(ctx),
	      _border(ctx.create_texture("button")),
	      _label(ctx.create_text(label)),
	      _clicked(clicked), _focused(focused) {
	}

	void Button::draw(bool active) {
		auto clip = active
			? vec4{0, 1.f/3, 1, 1.f/3}
			: vec4{0,     0, 1, 1.f/3};

		auto pos = position() + size()/2.f;

		_ctx.draw(_border, pos, active, clip);

		_ctx.draw(_label, pos, active);

		if(active && !_last_active) {
			_ctx.play_focus();

			if(_focused)
				_focused();
		}

		_last_active = active;
	}

	void Button::on_activate() {
		_ctx.play_activate();

		if(_clicked)
			_clicked();
	}

	auto Button::size()const noexcept -> glm::vec2 {
		return {_border->width(), _border->height()/3.f};
	}

}
}
