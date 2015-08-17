#include "widget.hpp"

namespace mo {
namespace gui {

	using namespace glm;

	void Widget::position(vec2 p) noexcept {
		_position = p;
		_on_moved(p);
	}

	auto Widget::contains(vec2 p) const noexcept -> bool {
		auto min = position();
		auto max = min+size();

		return p.x>=min.x && p.y>=min.y &&
		       p.x<=max.x && p.y<=max.y;
	}


	Widget_container::Widget_container(
	        Ui_ctx& ctx,
	        vec2 position, vec2 size, Layout layout)
	    : Widget(ctx, position), _size(size),
	      _auto_size_x(_size.x<=0),
	      _auto_size_y(_size.y<=0),
	      _layout(layout) {
	}

	auto Widget_container::add(Widget_ptr w) -> Widget_container& {
		_children.push_back(std::move(w));
		_realign();
		return *this;
	}

	void Widget_container::_realign() {
		_layout(position(), _size, _children);
		if(_auto_size_x || _auto_size_y) {
			auto max_p = position();

			for(auto& c : _children) {
				auto p = c->position() + c->size();

				if(max_p.x<p.x) max_p.x = p.x;
				if(max_p.y<p.y) max_p.y = p.y;
			}

			if(_auto_size_x)
				_size.x = max_p.x - position().x;
			if(_auto_size_y)
				_size.y = max_p.y - position().y;
		}
	}

	void Widget_container::draw(bool active) {
		int i=0;
		for(auto& c : _children)
			c->draw(i++==_focus && active);
	}
	void Widget_container::update(float dt) {
		for(auto& c : _children)
			c->update(dt);
	}

	void Widget_container::on_activate() {
		_children.at(_focus)->on_activate();
	}
	void Widget_container::on_input(const std::string& c) {
		_children.at(_focus)->on_input(c);
	}
	void Widget_container::on_move(vec2 point) {
		std::size_t i=0;
		for(auto& c : _children) {
			if(c->contains(point)) {
				_focus = i;
				c->on_move(point);
				return;
			}

			++i;
		}
	}

	void Widget_container::on_move(Direction dir) {
		if(dir==Direction::left || dir==Direction::up)
			_focus--;
		else
			_focus++;

		if(_focus<0)
			_focus = _children.size()-1;
		else if(_focus>=int(_children.size()))
			_focus = 0;

		_children.at(_focus)->on_move(dir);
	}

	void Widget_container::_on_moved(glm::vec2 p) noexcept {
		_realign();
	}


	auto vertical  (float padding) -> Layout {
		return [padding](glm::vec2 pos, vec2 size, Widget_list& widgets) {
			pos += vec2{0, padding};

			for(auto& w : widgets) {
				w->position(pos);
				pos.y += w->size().y + padding;
			}
		};
	}

	auto horizontal(float padding) -> Layout {
		return [padding](glm::vec2 pos, vec2 size, Widget_list& widgets) {
			pos += vec2{padding, 0};

			for(auto& w : widgets) {
				w->position(pos);
				pos.x += w->size().x + padding;
			}
		};
	}

}
}
