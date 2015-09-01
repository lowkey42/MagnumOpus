#pragma once

#include <vector>
#include <unordered_map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <utility>

#include <moodycamel/concurrentqueue.hpp>
#include "reflection.hpp"
#include "log.hpp"
#include "maybe.hpp"
#include "template_utils.hpp"

namespace mo {
namespace util {

	class Message_bus;

	template<class T>
	class Mailbox {
		public:
			Mailbox(Message_bus& bus, std::size_t size=128);
			Mailbox(Message_bus&&) = delete;
			~Mailbox();

			void send(const T& v);
			auto receive() -> maybe<T>;
			template<std::size_t Size>
			auto receive(T (&target)[Size]) -> std::size_t;

			auto empty()const noexcept -> bool;

		private:
			moodycamel::ConcurrentQueue<T> _queue;
			Message_bus& _bus;
	};


	class Message_bus {
		public:
			Message_bus();
			Message_bus(Message_bus&&) = default;
			~Message_bus();
			auto create_child() -> Message_bus;

			template<typename T>
			void register_mailbox(Mailbox<T>& mailbox, Typeid_type self=0);
			template<typename T>
			void unregister_mailbox(Mailbox<T>& mailbox);

			void update();

			template<typename Msg, typename... Arg>
			void send(Arg&&... arg) {
				send<Msg>(notypeid, std::forward<Arg>(arg)...);
			}
			template<typename Msg, typename... Arg>
			void send(Typeid_type self, Arg&&... arg) {
				send_msg(Msg{std::forward<Arg>(arg)...}, self);
			}

			template<typename Msg>
			void send_msg(const Msg& msg, Typeid_type self);

		private:
			Message_bus(Message_bus* parent);

			struct Mailbox_ref {
				template<typename T>
				Mailbox_ref(Mailbox<T>& mailbox, Typeid_type self=0) ;

				template<typename T>
				void exec_send(const T& m, Typeid_type self);

				bool operator==(const Mailbox_ref& rhs)const noexcept {
					return _type==rhs._type && _mailbox==rhs._mailbox;
				}

				Typeid_type _self;
				Typeid_type _type;
				void* _mailbox;
				std::function<void(void*, const void*)> _send;
			};

			Message_bus* _parent;
			std::vector<Message_bus*> _children;

			std::unordered_map<Typeid_type, std::vector<Mailbox_ref>> _mb_groups; // TODO: better datastructure? all keys known at compiletime but sparse

			std::vector<Mailbox_ref> _add_queue;
			std::vector<Mailbox_ref> _remove_queue;
	};

}
}

#define MESSAGEBUS_HPP_INCLUDED
#include "messagebus.hxx"

