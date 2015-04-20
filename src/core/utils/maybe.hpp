/**************************************************************************\
 * simple wrapper for optional values or references                       *
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

#include <stdexcept>
#include <functional>
#include <memory>
#include "log.hpp"

namespace core {
namespace util {

	template<typename T>
	class maybe {
		public:
			/*implicit*/ maybe(T&& data)noexcept : _valid(true), _data(std::move(data)) {}
			/*implicit*/ maybe(const T& data)noexcept : _valid(true), _data(data) {}
			maybe(const maybe& o)noexcept : _valid(o._valid), _data(o._data) {}
			maybe(maybe&& o)noexcept : _valid(o._valid), _data(std::move(o._data)) {
				o._valid = false;
			}
			~maybe()noexcept {
				if(is_some())
					_data.~T();
			}

			operator maybe<const T>()const noexcept {
				return is_some() ? maybe<const T>(_data) : maybe<const T>::nothing();
			}

			maybe& operator=(const maybe& o)noexcept {
				_valid = o._valid;
				_data = o._data;
				return *this;
			}
			maybe& operator=(maybe&& o)noexcept {
				_valid = o._valid;
				if(o._valid) {
					if(_valid)
						_data = o._data;
					else
						_data.T(_data);
				}

				o._valid = false;
				o._data.~T();
				return *this;
			}

			static maybe nothing() noexcept {
				return maybe();
			}


			bool is_some()const noexcept {
				return _valid;
			}
			bool is_nothing()const noexcept {
				return !is_some();
			}

			T& get_or_throw() {
				INVARIANT(is_some(), "Called getOrThrow on nothing.");

				return _data;
			}
			const T& get_or_throw()const {
				INVARIANT(is_some(), "Called getOrThrow on nothing.");

				return _data;
			}
			T& get_or_other(T& other)const noexcept {
				return is_some() ? _data : other;
			}

			template<typename Func>
			void process(Func f) {
				if(is_some())
					f(_data);
			}

		private:
			maybe() : _valid(false) {}

			bool _valid;
			union {
				T _data;
			};
	};

	struct nothing {
		template<typename T>
		operator maybe<T>()const noexcept {
			return maybe<T>::nothing();
		}
	};

	template<typename T>
	maybe<T> just(T&& inst) {
		return maybe<T>(std::move(inst));
	}
	template<typename T>
	maybe<T> justCopy(const T& inst) {
		return maybe<T>(inst);
	}
	template<typename T>
	maybe<T&> justPtr(T* inst) {
		return inst!=nullptr ? maybe<T&>(*inst) : nothing();
	}

	template<typename T, typename Func>
	auto operator>>(const maybe<T>& t, Func f) ->  maybe<decltype(f(t.get_or_throw()))> {
		return t.is_some() ? just(f(t.get_or_throw())) : nothing();
	}

	template<typename T>
	bool operator! (const maybe<T>& m) {
		return !m.is_some();
	}


	template<typename T>
	class maybe<T&> {
		public:
			maybe() : _ref(nullptr) {}
			/*implicit*/ maybe(T& data)noexcept : _ref(&data) {}
			maybe(const maybe& o)noexcept : _ref(o._ref) {}
			maybe(maybe&& o)noexcept : _ref(o._ref) {
				o._ref = nullptr;
			}
			~maybe()noexcept = default;

			operator maybe<const T&>()const noexcept {
				return is_some() ? maybe<const T&>(*_ref) : maybe<const T&>::nothing();
			}

			maybe& operator=(const maybe& o)noexcept {
				_ref = o._ref;
				return *this;
			}
			maybe& operator=(maybe&& o)noexcept {
				std::swap(_ref=nullptr, o._ref);
				return *this;
			}

			static maybe nothing() noexcept {
				return maybe();
			}

			bool is_some()const noexcept {
				return _ref!=nullptr;
			}
			bool is_nothing()const noexcept {
				return !is_some();
			}

			T& get_or_throw()const {
				INVARIANT(is_some(), "Called getOrThrow on nothing.");

				return *_ref;
			}
			T& get_or_other(T& other)const noexcept {
				return is_some() ? *_ref : other;
			}

			template<typename Func>
			void process(Func f) {
				if(is_some())
					f(get_or_throw());
			}

		private:
			T* _ref;
	};

	namespace details {
		template<int ...>
		struct seq { };

		template<int N, int ...S>
		struct gens : gens<N-1, N-1, S...> { };

		template<int ...S>
		struct gens<0, S...> {
		  typedef seq<S...> type;
		};

		// FIXME: broken
		template<typename... T>
		struct processor {
			std::tuple<T&&...> args;

			template<typename Func>
			void operator>>(Func&& f) {
				call(std::forward<Func>(f), typename gens<sizeof...(T)>::type());
			}

			private:
				template<typename Func, int ...S>
				void call(Func&& f, seq<S...>) {
					call(std::forward<Func>(f), std::forward<decltype(std::get<S>(args))>(std::get<S>(args))...);
				}

				template<typename Func>
				void call(Func&& f, T&&... m) {
					for(bool b : {m.is_some()...})
						if(!b)

					f(m.get_or_throw()...);
				}
		};
	}

	/*
	 * Usage:
	 * maybe<bool> b = true;
	 *	maybe<int> i = nothing();
	 *	maybe<float> f = 1.0f;
	 *
	 *	process(b,i,f)>> [](bool b, int i, float& f){
	 *		// ...
	 *	};
	 */
	template<typename... T>
	auto process(T&&... m) -> details::processor<T...> {
		return details::processor<T...>{std::tuple<decltype(m)...>(std::forward<T>(m)...)};
	}

	template<typename T>
	class lazy {
		public:
			using source_t = std::function<T()>;

			/*implicit*/ lazy(source_t s) : _source(s){}

			operator T(){
				return _source;
			}

		private:
			source_t _source;
	};

	template<typename T>
	inline lazy<T> later(typename lazy<T>::source_t f) {
		return lazy<T>(f);
	}


	template <class F>
	struct return_type;

	template <class R, class T, class... A>
	struct return_type<R (T::*)(A...)>
	{
	  typedef R type;
	};
	template <class R, class... A>
	struct return_type<R (*)(A...)>
	{
	  typedef R type;
	};

	template<typename S, typename T>
	inline lazy<T> later(S* s, T (S::*f)()) {
		std::weak_ptr<S> weak_s = s->shared_from_this();

		return lazy<T>([weak_s, f](){
			auto shared_s = weak_s.lock();
			if(shared_s) {
				auto s = shared_s.get();
				return (s->*f)();

			} else {
				return T{};
			}
		});
	}

}
}

