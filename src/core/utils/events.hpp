/*******************************************************************************\
 * signal/slot implementation                                                  *
 *                                               ___                           *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___          *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|         *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \         *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/         *
 *                |___/                              |_|                       *
 *                                                                             *
 * Copyright (c) 2014 Florian Oetke                                            *
 *                                                                             *
 *  This file is part of MagnumOpus and distributed under the MIT License      *
 *  See LICENSE file for details.                                              *
\*******************************************************************************/

#pragma once

#include <functional>
#include <vector>
#include <algorithm>

namespace mo {
namespace util {

	template<typename... ET>
	class slot;

	template<typename... ET>
	class signal_source {
		friend class slot<ET...>;
		public:
			void inform(ET... e) {
				for(auto&& s : _slots)
					if(s->func)
						s->func(e...);
			}

		private:
			void register_slot(slot<ET...>* s) {
				_slots.push_back(s);
			}

			void unregister_slot(slot<ET...>* s) {
				auto e = std::find(_slots.begin(), _slots.end(), s);
				*e = _slots.back();
				_slots.pop_back();
			}

			std::vector<slot<ET...>*> _slots;
	};

	template<typename... ET>
	class slot {
		friend class signal_source<ET...>;
		public:
			template<typename Base>
			slot(void (Base::*f)(ET...), Base* inst )
			  : func([f, inst](ET... arg){(inst->*f)(arg...);}) {}
			slot(std::function<void(ET...)> f) : func(f) {}
			~slot() {
				for(auto&& s : connections)
					s->unregister_slot(this);
			}

			void connect(signal_source<ET...>& source) {
				source.register_slot(this);
				connections.push_back(&source);
			}
			void disconnect(signal_source<ET...>& source) {
				source.unregister_slot(this);

				auto e = std::find(connections.begin(), connections.end(), &source);
				*e = connections.back();
				connections.pop_back();
			}

		private:
			const std::function<void(ET...)> func;
			std::vector<signal_source<ET...>*> connections;
	};

}
}
