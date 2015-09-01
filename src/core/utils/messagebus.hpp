#pragma once

#include <vector>
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
			void register_mailbox(Mailbox<T>& mailbox, Typeuid self=0);
			template<typename T>
			void unregister_mailbox(Mailbox<T>& mailbox);

			void update();

			template<typename Msg, typename... Arg>
			void send(Arg&&... arg) {
				send<Msg>(util::typeuid_of<void>(), std::forward<Arg>(arg)...);
			}
			template<typename Msg, typename... Arg>
			void send(Typeuid self, Arg&&... arg) {
				send_msg(Msg{std::forward<Arg>(arg)...}, self);
			}

			template<typename Msg>
			void send_msg(const Msg& msg, Typeuid self);

		private:
			Message_bus(Message_bus* parent);

			struct Mailbox_ref {
				template<typename T>
				Mailbox_ref(Mailbox<T>& mailbox, Typeuid self=0) ;

				template<typename T>
				void exec_send(const T& m, Typeuid self);

				bool operator==(const Mailbox_ref& rhs)const noexcept {
					return _type==rhs._type && _mailbox==rhs._mailbox;
				}

				Typeuid _self;
				Typeuid _type;
				void* _mailbox;
				std::function<void(void*, const void*)> _send;
			};

			auto& group(Typeuid id) {
				if(std::size_t(id)>=_mb_groups.size()) {
					_mb_groups.resize(id+1);
					_mb_groups.back().reserve(4);
				}
				return _mb_groups[id];
			}

			Message_bus* _parent;
			std::vector<Message_bus*> _children;

			std::vector<std::vector<Mailbox_ref>> _mb_groups;

			std::vector<Mailbox_ref> _add_queue;
			std::vector<Mailbox_ref> _remove_queue;
	};

}
}

#define MESSAGEBUS_HPP_INCLUDED
#include "messagebus.hxx"

