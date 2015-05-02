#include "ecs.hpp"

#include <algorithm>
#include <stdexcept>

#include "../utils/string_utils.hpp"
#include "../utils/log.hpp"
#include <iostream>

#include "component.hpp"
#include "serializer.hpp"

namespace mo {
namespace ecs {


	struct Entity_constructor : Entity {
		Entity_constructor(Entity_manager& e) : Entity(e){}
	};


	Entity_manager::Entity_manager(asset::Asset_manager& asset_mgr)
		: _unoptimized_deletions(0), _serializer(std::make_unique<Serializer>(*this,asset_mgr)) {
	}

	Entity_ptr Entity_manager::emplace()noexcept {
		auto e = std::make_shared<Entity_constructor>(*this);
		_entities.push_back(e);

		return e;
	}
	Entity_ptr Entity_manager::emplace(const asset::AID& blueprint)noexcept {
		auto e = emplace();

		_serializer->apply(blueprint, e);

		return e;
	}

	void Entity_manager::erase(Entity_ptr ref) {
		if(std::find(_delete_queue.begin(), _delete_queue.end(), ref)!=_delete_queue.end()) {
			ERROR("Double-Deletion of entity "<<ref.get());
			return;
		}

		_delete_queue.push_back(ref);
	}

	void Entity_manager::process_queued_actions() {
		constexpr unsigned int resize_after_n_deletions = 50;

		auto new_end = std::remove_if(std::begin(_entities), std::end(_entities),
			[&](Entity_ptr& v){
				bool remove = std::find(_delete_queue.begin(), _delete_queue.end(), v)!=_delete_queue.end();

				if(remove) {
					for(auto& cp : _pools) {
						if(cp)
							cp->free(*v);
					}
				}

				return remove;
		} );

		_entities.erase(new_end, _entities.end() );

		_delete_queue.clear();

		for(auto& cp : _pools) {
			if(cp)
				cp->process_queued_actions();
		}

		if(_unoptimized_deletions>=resize_after_n_deletions) {
			shrink_to_fit();

			_unoptimized_deletions = 0;
		}
	}
	void Entity_manager::shrink_to_fit() {
		for(auto& cp : _pools)
			if(cp)
				cp->shrink_to_fit();
	}

	void Entity_manager::clear() {
		for(auto& cp : _pools)
			if(cp)
				cp->clear();

		_entities.clear();
		_delete_queue.clear();
	}

	auto Entity_manager::comp_info(const std::string& name)const -> const details::Component_type_info& {
		auto ti = _types.find(name);
		if(ti!=_types.end())
			return ti->second;
		else
			FAIL("Unknown component: "<<name);
	}

} /* namespace ecs */
}
