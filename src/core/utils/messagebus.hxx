#pragma once

#ifndef MESSAGEBUS_HPP_INCLUDED
	#include "messagebus.hpp"
#endif

namespace mo {
namespace util {

	template<class T>
	Mailbox<T>::Mailbox(Message_bus& bus, std::size_t size) : _queue(size,0,4), _bus(bus) {
		_bus.register_mailbox(*this);
	}

	template<class T>
	Mailbox<T>::~Mailbox() {
		_bus.unregister_mailbox(*this);
	}

	template<class T>
	void Mailbox<T>::send(const T& v) {
		_queue.enqueue(v);
	}

	template<class T>
	auto Mailbox<T>::receive() -> maybe<T> {
		maybe<T> ret = T{};

		if(!_queue.try_dequeue(ret.get_or_throw()))
			ret = nothing();

		return ret;
	}

	template<class T>
	template<std::size_t Size>
	auto Mailbox<T>::receive(T (&target)[Size]) -> std::size_t {
		return try_dequeue_bulk(target, Size);
	}

	template<class T>
	auto Mailbox<T>::empty()const noexcept -> bool {
		return _queue.size_approx() <= 0;
	}


	template<typename T>
	Message_bus::Mailbox_ref::Mailbox_ref(Mailbox<T>& mailbox, Typeuid self)
	  : _self(self), _type(typeuid_of<T>()),
		_mailbox(static_cast<void*>(&mailbox)),
		_send(+[](void* mb, const void* m){
			static_cast<Mailbox<T>*>(mb)->enqueue(*static_cast<const T*>(m));
		}) {
	}

	template<typename T>
	void Message_bus::Mailbox_ref::exec_send(const T& m, Typeuid self) {
		assert(_type==typeid_of<T>() && "Types don't match");

		if(_self!=0 && self==_self) {
			return;
		}

		_send(_mailbox, static_cast<const void*>(&m));
	}

	template<typename T>
	void Message_bus::register_mailbox(Mailbox<T>& mailbox, Typeuid self) {
		// TODO: mutex
		_add_queue.emplace_back(mailbox, self);
	}

	template<typename T>
	void Message_bus::unregister_mailbox(Mailbox<T>& mailbox) {
		// TODO: mutex
		_remove_queue.emplace_back(mailbox);
	}

	inline void Message_bus::update() {
		for(auto& m : _add_queue) {
			group(m._type).emplace_back(std::move(m));
		}
		_add_queue.clear();

		for(auto& m : _remove_queue) {
			util::erase_fast(group(m._type), m);
		}
		_remove_queue.clear();
	}


	template<typename Msg>
	void Message_bus::send_msg(const Msg& msg, Typeuid self) {
		auto id = typeuid_of<Msg>();

		if(id <_mb_groups.size()) {
			for(auto& mb : _mb_groups[id]) {
				mb.exec_send(msg, self);
			}
		}

		for(auto& c : _children)
			c->send_msg(msg, self);
	}

	inline Message_bus::Message_bus() : _parent(nullptr) {
	}
	inline Message_bus::Message_bus(Message_bus* parent) : _parent(parent) {
		_parent->_children.push_back(this);
	}
	inline Message_bus::~Message_bus() {
		if(_parent) {
			util::erase_fast(_parent->_children, this);
		}
	}

	inline auto Message_bus::create_child() -> Message_bus {
		return Message_bus(this);
	}

}
}
