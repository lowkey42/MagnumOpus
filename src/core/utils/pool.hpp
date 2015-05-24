/**************************************************************************\
 * a resizable, semi-contiguous pool of memory                            *
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
#include <cstring>
#include "log.hpp"
#include "string_utils.hpp"
#include "template_utils.hpp"

namespace mo {
namespace util {

	template<class POOL>
	class pool_iterator;


	template<std::size_t _BytesPerElement, std::size_t _ElementsPerChunk>
	class pool {
		friend class pool_iterator<pool<_BytesPerElement, _ElementsPerChunk>>;
		public:
			static constexpr auto BytesPerElement = _BytesPerElement;
			static constexpr auto ElementsPerChunk = _ElementsPerChunk;
			using iterator = pool_iterator<pool<BytesPerElement, ElementsPerChunk>>;

			pool()noexcept : _usedElements(0) {}
			~pool() {
				for( auto& c : _chunks )
					delete[] c;
			}

			iterator begin()noexcept;
			iterator end()noexcept;

			void clear() {
				for( auto& c : _chunks )
					delete[] c;
				//	std::memset(c, 0, ElementsPerChunk*BytesPerElement);

				_chunks.clear();

				_usedElements=0;
			}
			void pop_back() {
				std::memset(get(_usedElements-1), 0xdead, BytesPerElement);
				_usedElements--;
			}
			char* back() {
				auto i = _usedElements-1;
				return _chunks[i / ElementsPerChunk] + (i % ElementsPerChunk) * BytesPerElement;
			}

			void shrink_to_fit() {
				while(_usedElements >= (_chunks.size()-2)*ElementsPerChunk) {
					delete[] _chunks[_chunks.size()-1];
					_chunks.pop_back();
				}
			}

			std::size_t push() {
				auto i = _usedElements++;
				if( i/ElementsPerChunk>=_chunks.size() )
					_chunks.push_back(new char[ElementsPerChunk*BytesPerElement]);

				return i;
			}

			std::size_t size()const noexcept {
				return _usedElements;
			}

			char* get(std::size_t i) {
				return const_cast<char*>(static_cast<const pool*>(this)->get(i));
			}
			const char* get(std::size_t i)const {
				INVARIANT(i<_usedElements, "Pool-Index out of bounds "+to_string(i)+">="+to_string(_usedElements));

				return _chunks[i / ElementsPerChunk] + (i % ElementsPerChunk) * BytesPerElement;
			}

		private:
			std::vector<char*> _chunks;
			std::size_t _usedElements;
	};

	template<class Pool>
	class pool_iterator {
		public:
			typedef std::bidirectional_iterator_tag  iterator_category;
			typedef char*                            value_type;
			typedef std::ptrdiff_t                   difference_type;
			typedef std::vector<char*>::iterator     chunk_iterator;

			pool_iterator(Pool& pool, std::size_t index) : _pool(pool), _index(index) {};

			value_type operator*() {return _pool.get(_index);}

			pool_iterator& operator++() {
				INVARIANT(_index<_pool.size(), "overflow in pool_iterator");
				++_index;
				return *this;
			}
			pool_iterator& operator--() {
				INVARIANT(_index>0, "underflow in pool_iterator");
				--_index;
				return *this;
			}

			pool_iterator operator++(int) {
				pool_iterator t = *this;
				++*this;
				return t;
			}

			pool_iterator operator--(int) {
				pool_iterator t = *this;
				--*this;
				return t;
			}

			bool operator==(const pool_iterator& o) const {
				return _index==o._index;
			}
			bool operator!=(const pool_iterator& o) const {
				return _index!=o._index;
			}
			bool operator<(const pool_iterator& o) const {
				return _index>o._index;
			}

		private:
			Pool& _pool;
			std::size_t _index;
	};

	template<std::size_t BytesPerElement, std::size_t ElementsPerChunk>
	auto pool<BytesPerElement, ElementsPerChunk>::begin()noexcept
			-> iterator {
		return iterator(*this, 0);
	}

	template<std::size_t BytesPerElement, std::size_t ElementsPerChunk>
	auto pool<BytesPerElement, ElementsPerChunk>::end()noexcept
			-> iterator {
		return iterator(*this, size());
	}

}
}

