#define MO_BUILD_SERIALIZER
#include "elements.hpp"

#include <core/utils/log.hpp>

namespace mo {
namespace level {

	namespace {
		Element next(const Elements& e, Element i) {
			auto idx = static_cast<std::size_t>(i);

			if(idx<element_count) {
				do {
					idx++;
					i = static_cast<Element>(idx);

				} while(!(e.value & mask(i)) && idx<element_count);
			}

			return i;
		}
	}

	std::ostream& operator<<(std::ostream& s, Element e) {
		switch(e) {
			case Element::neutral:
				s<<"neutral";
				break;

			case Element::fire:
				s<<"fire";
				break;

			case Element::frost:
				s<<"frost";
				break;

			case Element::water:
				s<<"water";
				break;

			case Element::stone:
				s<<"stone";
				break;

			case Element::gas:
				s<<"gas";
				break;

			case Element::lightning:
				s<<"lightning";
				break;
		}

		return s;
	}

	Elements_iterator::Elements_iterator(const Elements& e, std::size_t i)
	    : e(&e), i(next(e, static_cast<Element>(i))) {
	}

	auto Elements_iterator::operator*() const -> const Element& {
		return i;
	}
	auto Elements_iterator::operator->() const -> const Element* {
		return &i;
	}

	auto Elements_iterator::operator++() -> Elements_iterator& {
		i = next(*e, i);
		return *this;
	}
	auto Elements_iterator::operator++(int) -> Elements_iterator {
		auto old = *this;

		++*this;

		return old;
	}

	bool Elements_iterator::operator==(const Elements_iterator& rhs) {
		return i==rhs.i && e==rhs.e;
	}

	bool Elements_iterator::operator!=(const Elements_iterator& rhs) {
		return i!=rhs.i || e!=rhs.e;
	}

}
}
