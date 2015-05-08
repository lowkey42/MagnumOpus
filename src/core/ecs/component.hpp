/**************************************************************************\
 * Basic types, e.g. handles for entities & components                    *
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
#define ECS_COMPONENT_INCLUDED

#include "../utils/template_utils.hpp"
#include "../utils/log.hpp"
#include "../utils/pool.hpp"
#include "../utils/events.hpp"

#include <vector>
#include <memory>

namespace mo {
namespace ecs {

	class Entity;
	using Entity_weak_ptr = std::weak_ptr<Entity>;
	using Entity_ptr = std::shared_ptr<Entity>;
	template<typename T> class Component_pool;
	class Entity_state;


	using Component_type = uint16_t;


	namespace details {
		constexpr Component_type max_comp_type = 16;

		class Component_base : public util::no_copy {
			public:
				Component_base(Entity& owner)noexcept;
				Component_base(Component_base&& o)noexcept;

				Component_base& operator=(Component_base&& o)noexcept;

				auto owner_ptr()const -> Entity_ptr;
				auto owner()const noexcept -> Entity& {return *_owner;}
				bool valid()const noexcept {
					return _owner!=nullptr;
				}

				virtual void load(Entity_state& state){}
				virtual void store(Entity_state& state){}

			protected:
				static Component_type _next_type_id()noexcept;

				virtual ~Component_base()noexcept=default;

				void _reg_self(Component_type type);
				void _unreg_self(Component_type type);

				Entity* _owner;
		};

		extern Component_base*& get_component(Entity& e, Component_type t);
		extern Entity_ptr get_entity(Entity& e);
	}

	template<typename T>
	using is_component = std::is_base_of<ecs::details::Component_base, T>;

	template<typename T>
	class Component : public details::Component_base {
		public:
			using Pool = Component_pool<T>;
			static constexpr std::size_t pool_chunk_size_bytes = 8192;
			static constexpr std::size_t min_components_per_pool_chunk = 512;


			static Component_type type();

			Component(Entity& owner)noexcept;
			Component(Component&& o)noexcept;
			Component& operator=(Component&& o)noexcept;

		protected:
			~Component()noexcept;

		private:
			using details::Component_base::_next_type_id;
			using details::Component_base::_reg_self;
			using details::Component_base::_unreg_self;
			using details::Component_base::_owner;
	};


	enum class Component_event_type {
		created,
		freed,
	};
	struct Component_event {
		Component_event_type type;
		Entity& handle;
	};

	class Component_pool_base {
		public:
			virtual ~Component_pool_base()noexcept = default;
			virtual void free(Entity& owner) = 0;
			virtual void clear() = 0;
			virtual void shrink_to_fit() = 0;
			virtual void process_queued_actions() = 0;
	};

	template<typename T>
	class Component_pool : public Component_pool_base, public util::signal_source<Component_event>, util::no_copy_move {
		public:
			using pool_type = util::pool<sizeof(T),	util::max(T::pool_chunk_size_bytes/sizeof(T), T::min_components_per_pool_chunk)>;
			using iterator = util::cast_iterator<typename pool_type::iterator, T>;


			Component_pool()=default;
			~Component_pool()noexcept;

			template<typename... Args>
			T& create(Entity& owner, Args&&... args);
			void free(Entity& owner);
			void clear();

			void shrink_to_fit();
			void process_queued_actions();

			iterator begin() {
				return iterator(_pool.begin());
			}

			iterator end() {
				return iterator(_pool.end());
			}
			std::size_t size()const noexcept {
				return _pool.size();
			}

		private:
			pool_type _pool;
			std::vector<T*> _delete_queue;
	};

} /* namespace ecs */
}

#include "component.hxx"
