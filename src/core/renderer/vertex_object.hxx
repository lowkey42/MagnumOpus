#pragma once

#ifndef VERTEX_OBJECT_INCLUDED
#include "vertex_object.hpp"
#endif

namespace mo {
namespace renderer {

	template<class T>
	void Buffer::set(const std::vector<T>& container) {
		_set_raw(sizeof(T), container.size(), &container[0]);
	}
	template<class T>
	void Buffer::set(typename std::vector<T>::const_iterator begin,
	                 typename std::vector<T>::const_iterator end) {
		_set_raw(sizeof(T), std::distance(begin, end), &*begin);
	}

	template<class T>
	Buffer create_dynamic_buffer(std::size_t elements) {
		return Buffer{sizeof(T), elements, true};
	}

	template<class T>
	Buffer create_buffer(const std::vector<T>& container, bool dynamic) {
		return Buffer{sizeof(T), container.size(), dynamic, &container[0]};
	}

	namespace details {
		template<class P, class M>
		const void* calcOffset(const M P::*member) {
			return reinterpret_cast<const void*>(&( reinterpret_cast<P*>(0)->*member));
		}
	}

#define VERTEX_FACTORY(TYPE,NAME,COUNT) \
	template<class Base> \
	Vertex_layout::Element vertex(const std::string& name, TYPE Base::* value, std::size_t buffer, uint8_t divisor, bool normalized) { \
		return Vertex_layout::Element{name, COUNT, Vertex_layout::Element_type::NAME, normalized, details::calcOffset(value), buffer, divisor};\
	}

	VERTEX_FACTORY(int8_t,    byte_t,   1)
	VERTEX_FACTORY(uint8_t,   ubyte_t,  1)
	VERTEX_FACTORY(int16_t,   short_t,  1)
	VERTEX_FACTORY(uint16_t,  ushort_t, 1)
	VERTEX_FACTORY(int32_t,   int_t,    1)
	VERTEX_FACTORY(uint32_t,  uint_t,   1)
	VERTEX_FACTORY(float,     float_t,  1)
	VERTEX_FACTORY(glm::vec2, float_t,  2)
	VERTEX_FACTORY(glm::vec3, float_t,  3)
	VERTEX_FACTORY(glm::vec4, float_t,  4)

#undef VERTEX_FACTORY


	template<class... B>
	Object::Object(const Vertex_layout& layout, B&&... d)
		: _mode(layout._mode), _data(), _vao_id(0) {
		_data.reserve(sizeof...(d));
		auto ignored = {(_data.emplace_back(std::forward<B>(d)), 0)...};
		(void)ignored;

		_init(layout);
	}

}
}
