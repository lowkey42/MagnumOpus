#pragma once

#ifndef ECS_COMPONENT_INCLUDED
#include "component.hpp"
#endif

namespace core {
namespace ecs {

	template<typename T>
	Component_type Component<T>::type() {
		static Component_type t = _next_type_id();
		return t;
	}
	template<typename T>
	Component<T>::Component(Entity& owner)noexcept : Component_base(owner) {
		_reg_self(T::type());
	}
	template<typename T>
	Component<T>::Component(Component&& o)noexcept : Component_base(o) {
		_reg_self(T::type());
	}
	template<typename T>
	Component<T>::~Component()noexcept {
		_unreg_self(T::type());
	}
	template<typename T>
	Component<T>& Component<T>::operator=(Component&& o)noexcept {
		if(this!=&o) {
			_owner = o._owner;
			o._owner = nullptr;
			_reg_self(T::type());
		}
		return *this;
	}


	template<typename T>
	Component_pool<T>::~Component_pool()noexcept {
		clear();
	}

	template<typename T>
	template<typename... Args>
	T& Component_pool<T>::create(Entity& owner, Args&&... args) {
		const std::size_t index = _pool.push();

		char* mem = _pool.get(index);
		T* addr = new(mem) T(owner, std::forward<Args>(args)...);

		this->inform(Component_event{Component_event_type::created, owner});

		return *addr;
	}

	template<typename T>
	void Component_pool<T>::free(Entity& owner) {
		this->inform(Component_event{Component_event_type::freed, owner});

		auto comp = details::get_component(owner, T::type());

		if(comp) {
			T& cptr = *static_cast<T*>(comp);

			INVARIANT(cptr.valid(), "double free");

			_delete_queue.push_back(&cptr);
		};
	}

	template<typename T>
	void Component_pool<T>::process_queued_actions() {
		for(auto&& e : _delete_queue) {
			INVARIANT(e->valid(), "double free");

			e->~T();

			T& back = *reinterpret_cast<T*>(_pool.back());
			if(e!=&back) {
				*e = std::move(back);
			}
			_pool.pop_back();
		}

		_delete_queue.clear();
	}

	template<typename T>
	void Component_pool<T>::clear() {
		for(auto i=_pool.size(); i>0; i--) {
			auto comp = reinterpret_cast<T*>(_pool.get(i-1));
			auto& e = comp->owner();
			this->inform(Component_event{Component_event_type::freed, e});

			comp->~T();
		}
		_pool.clear();
	}

	template<typename T>
	void Component_pool<T>::shrink_to_fit() {
		_pool.shrink_to_fit();
	}

}
}
