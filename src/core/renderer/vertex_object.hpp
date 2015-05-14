/**************************************************************************\
 * Wrappers for VAOs & VBOs                                               *
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

#include <cinttypes>
#include <vector>
#include <iterator>
#include <string>
#include <glm/glm.hpp>

#include "../utils/template_utils.hpp"

/*
 * Examples:
	struct FancyData {
		glm::vec2 pos;
		bool isMagic;
	};

	std::vector<FancyData> data;

	auto buffer = createBuffer(data, true);
	VertexLayout layout{
		VertexLayout::Mode::Point,
		vertex("position",  &FancyData::pos),
		vertex("magic",     &FancyData::isMagic)
	};

	auto shader = Shader_program{}
		.attachShader(assetMgr.load<Shader>("frag_shader:example"_aid))
		.attachShader(assetMgr.load<Shader>("vert_shader:example"_aid))
		.bindAllAttributeLocations(layout)
		.build();

	Object obj{layout, {buffer}};

	shader.set_uniform("liveMagic", 0.42f)
	      .bind();

	obj.draw();
	data.push_back({1,2,3,true}); //< update data
	obj.getBuffer().set(data);

	obj.draw(); //< draws updated data
 */
namespace mo {
namespace renderer {

	class Object;
	class Shader_program;


	class Buffer : util::no_copy {
		friend class Object;
		public:
			Buffer(std::size_t element_size, std::size_t elements,
			       bool dynamic, const void* data=nullptr);
			Buffer(Buffer&& b)noexcept;
			~Buffer()noexcept;

			template<class T>
			void set(const std::vector<T>& container);

			template<class T>
			void set(typename std::vector<T>::const_iterator begin, typename std::vector<T>::const_iterator end);

			Buffer& operator=(Buffer&& b)noexcept;

			std::size_t size()const noexcept{return _elements;}

		private:
			unsigned int _id;
			std::size_t _element_size;
			std::size_t _elements;
			std::size_t _max_elements;
			bool _dynamic;

			void _set_raw(std::size_t element_size, std::size_t size, const void* data);
			void _bind()const;
	};
	template<class T>
	Buffer create_dynamic_buffer(std::size_t elements);

	template<class T>
	Buffer create_buffer(const std::vector<T>& container, bool dynamic=false);


	class Vertex_layout {
		friend class Object;
		public:
			enum class Mode {
				points,
				line_strip,
				line_loop,
				lines,
				triangle_strip,
				triangle_fan,
				triangles
			};

			enum class Element_type {
				byte_t, ubyte_t, short_t, ushort_t, int_t, uint_t, float_t, double_t
			};

			struct Element {
				std::string name;
				int size;
				Element_type type;
				bool normalized;
				const void* offset;
			};

		public:
			template<typename ...T>
			Vertex_layout(Mode mode, T&&... elements)
			    : _mode(mode), _elements{std::forward<T>(elements)...} {}

			void setup_shader(Shader_program& shader)const;


		private:
			const Mode _mode;
			const std::vector<Element> _elements;

			void _build(std::size_t stride)const;
	};
	template<class Base> Vertex_layout::Element vertex(const std::string& name, int8_t    Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, uint8_t   Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, int16_t   Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, uint16_t  Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, int32_t   Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, uint32_t  Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, float     Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, double    Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, glm::vec2 Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, glm::vec3 Base::* value, bool normalized=false);
	template<class Base> Vertex_layout::Element vertex(const std::string& name, glm::vec4 Base::* value, bool normalized=false);


	class Object : util::no_copy {
		public:
			Object(const Vertex_layout& layout, Buffer&& data);
			Object(Object&&)noexcept;
			~Object()noexcept;

			void draw()const;

			Buffer& buffer(){return _data;}

			Object& operator=(Object&&)noexcept;

		private:
			Vertex_layout::Mode _mode;
			Buffer _data;
			unsigned int _vao_id;
	};

}
}

#define VERTEX_OBJECT_INCLUDED
#include "vertex_object.hxx"

