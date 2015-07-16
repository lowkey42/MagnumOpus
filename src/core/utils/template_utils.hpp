/**************************************************************************\
 * small helpers for template programming                                 *
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
#include <algorithm>
#include <functional>

namespace mo {
namespace util {

	struct no_move {
		protected:
			no_move() = default;
			~no_move()noexcept = default;
			no_move(no_move&&)=delete;
			no_move& operator=(no_move&&)=delete;
	};
	struct no_copy {
		protected:
			no_copy() = default;
			~no_copy()noexcept = default;
			no_copy(const no_copy&)=delete;
			no_copy& operator=(const no_copy&)=delete;
	};
	struct no_copy_move : no_copy, no_move {
		protected:
			no_copy_move() = default;
			~no_copy_move()noexcept = default;
	};

	template<class Func>
	struct cleanup {
		cleanup(Func f)noexcept : active(true), f(f) {}
		cleanup(const cleanup&)=delete;
		cleanup(cleanup&& o)noexcept : active(o.active), f(o.f) {o.active=false;}
		~cleanup()noexcept{f();}

		bool active;
		Func f;
	};
	template<class Func>
	inline auto cleanup_later(Func f)noexcept {
		return cleanup<Func>{f};
	}

	template<typename T>
	constexpr bool dependent_false() {
		return false;
	}

	template<typename T>
	constexpr T max(T a, T b) {
		return a<b ? b : a;
	}

	template<typename T>
	void erase_fast(std::vector<T>& c, const T& v) {
		using std::swap;

		auto e = std::find(c.begin(), c.end(), v);
		if(e!=c.end()) {
			swap(*e, c.back());
			c.pop_back();
		}
	}
	template<typename T>
	void erase_fast_stable(std::vector<T>& c, const T& v) {
		auto ne = std::remove(c.begin(), c.end(), v);

		if(ne!=c.end()) {
			c.erase(ne, c.end());
		}
	}

	template< typename ContainerT, typename PredicateT >
	void erase_if(ContainerT& items, const PredicateT& predicate) {
		for( auto it = items.begin(); it != items.end(); ) {
			if( predicate(*it) ) it = items.erase(it);
			else ++it;
		}
	}

	template<class Iter>
	class iter_range {
		public:
			iter_range()noexcept {}
			iter_range(Iter begin, Iter end)noexcept : b(begin), e(end) {}

			bool operator==(const iter_range& o)noexcept {
				return b==o.b && e==o.e;
			}

			Iter begin()const noexcept {
				return b;
			}
			Iter end()const noexcept {
				return e;
			}

		private:
			Iter b, e;
	};
	template<class T>
	using vector_range = iter_range<typename std::vector<T>::iterator>;

	template<class T>
	using cvector_range = iter_range<typename std::vector<T>::const_iterator>;

	template<class T>
	class numeric_range {
		struct iterator : std::iterator<std::random_access_iterator_tag, T, T> {
			T p;
			T s;
			constexpr iterator(T v, T s=1)noexcept : p(v), s(s) {};
			constexpr iterator(const iterator&)noexcept = default;
			constexpr iterator(iterator&&)noexcept = default;
			iterator& operator++()noexcept {p+=s; return *this;}
			iterator operator++(int)noexcept {auto t=*this; *this++; return t;}
			iterator& operator--()noexcept {p-=s; return *this;}
			iterator operator--(int)noexcept {auto t=*this; *this--; return t;}
			bool operator==(const iterator& rhs)const noexcept {return p==rhs.p;}
			bool operator!=(const iterator& rhs)const noexcept {return p!=rhs.p;}
			const T& operator*()const noexcept {return p;}
		};
		using const_iterator = iterator;

		public:
			constexpr numeric_range()noexcept {}
			constexpr numeric_range(T begin, T end, T step=1)noexcept : b(begin), e(end), s(step) {}
			constexpr numeric_range(numeric_range&&)noexcept = default;
			constexpr numeric_range(const numeric_range&)noexcept = default;

			numeric_range& operator=(const numeric_range&)noexcept = default;
			numeric_range& operator=(numeric_range&&)noexcept = default;
			bool operator==(const numeric_range& o)noexcept {
				return b==o.b && e==o.e;
			}

			constexpr iterator begin()const noexcept {
				return b;
			}
			constexpr iterator end()const noexcept {
				return e;
			}

		private:
			T b, e, s;
	};
	template<class Iter, typename = std::enable_if_t<!std::is_arithmetic<Iter>::value> >
	constexpr iter_range<Iter> range(Iter b, Iter e) {
		return {b,e};
	}
	template<class T, typename = std::enable_if_t<std::is_arithmetic<T>::value> >
	constexpr numeric_range<T> range(T b, T e, T s=1) {
		return {b, std::max(e+1, b), s};
	}
	template<class T, typename = std::enable_if_t<std::is_arithmetic<T>::value> >
	constexpr numeric_range<T> range(T num) {
		return {0,static_cast<T>(num)};
	}
	template<class Container, typename = std::enable_if_t<!std::is_arithmetic<Container>::value> >
	iter_range<typename Container::iterator> range(Container& c) {
		using namespace std;
		return {begin(c),end(c)};
	}
	template<class Container, typename = std::enable_if_t<!std::is_arithmetic<Container>::value> >
	iter_range<typename Container::const_iterator> range(const Container& c) {
		using namespace std;
		return {begin(c),end(c)};
	}

	template<class Iter, class Type>
	class cast_iterator {
		public:
			typedef typename Iter::iterator_category  iterator_category;
			typedef Type                              value_type;
			typedef std::ptrdiff_t                    difference_type;
			typedef Type*                             pointer;
			typedef Type&                             reference;


			cast_iterator(Iter iter) : iter(iter) {};

			reference operator*() {return *reinterpret_cast<pointer>(*iter);}
			pointer operator->() {return reinterpret_cast<pointer>(&*iter);}

			cast_iterator& operator++() {
				++iter;
				return *this;
			}
			cast_iterator& operator--() {
				--iter;
				return *this;
			}

			cast_iterator operator++(int) {
				cast_iterator t = *this;
				++*this;
				return t;
			}

			cast_iterator operator--(int) {
				cast_iterator t = *this;
				--*this;
				return t;
			}

			bool operator==(const cast_iterator& o) const {
				return iter==o.iter;
			}
			bool operator!=(const cast_iterator& o) const {
				return iter!=o.iter;
			}
			bool operator<(const cast_iterator& o) const {
				return iter<o.iter;
			}


		private:
			Iter iter;
	};


	template<typename F>
	void doOnce(F f) {
		static bool first = true;
		if(first) {
			first = false;
			f();
		}
	}

}
}

