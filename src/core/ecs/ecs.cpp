#include "ecs.hpp"

#include <algorithm>
#include <stdexcept>

#include "../utils/string_utils.hpp"
#include "../utils/log.hpp"
#include <iostream>

#include "component.hpp"
#include "serializer.hpp"

#include <sf2/sf2.hpp>

namespace mo {
namespace ecs {


	struct Entity_constructor : Entity {
		Entity_constructor(Entity_manager& e) : Entity(e){}
	};


	Entity_manager::Entity_manager(asset::Asset_manager& asset_mgr)
		: _asset_mgr(asset_mgr), _unoptimized_deletions(0) {

		init_blueprints(*this);
	}

	Entity_ptr Entity_manager::emplace()noexcept {
		auto e = std::make_shared<Entity_constructor>(*this);
		_entities.push_back(e);

		return e;
	}
	Entity_ptr Entity_manager::emplace(const asset::AID& blueprint)noexcept {
		auto e = emplace();

		apply_blueprint(_asset_mgr, *e, blueprint);

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

		for(auto& cp : _pools) {
			if(cp)
				cp->process_queued_actions();
		}

		_entities.erase(new_end, _entities.end() );

		_delete_queue.clear();

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

	auto Entity_manager::comp_info(const std::string& name)const -> const details::Component_type_info& {
		auto ti = _types.find(name);
		if(ti!=_types.end())
			return ti->second;
		else
			FAIL("Unknown component: "<<name);
	}

	void Entity_manager::write(std::ostream& stream) {
		write(stream, _entities);
	}

	void Entity_manager::write(std::ostream& stream,
	                           const std::vector<Entity_ptr>& entities) {
		auto serializer = EcsSerializer{stream, *this, _asset_mgr};
		serializer.write_virtual(
			sf2::vmember("entities", entities)
		);

		stream.flush();
	}

	void Entity_manager::read(std::istream& stream, bool clear) {
		if(clear) {
			for(auto& cp : _pools)
				if(cp)
					cp->clear();

			_entities.clear();
			_delete_queue.clear();
		}

		auto entities = _entities;

		auto deserializer = EcsDeserializer{"$EntityDump", stream, *this, _asset_mgr};
		deserializer.read_virtual(
			sf2::vmember("entities", entities)
		);


		if(clear)
			_entities = std::move(entities);
		else
			_entities.push_back(entities.back());
	}

	auto save_entity(Entity_manager& manager, const Entity& entity) -> ETO {
		Entity& mutable_entity = const_cast<Entity&>(entity);
		std::stringstream stream;
		manager.write(stream, {mutable_entity.shared_from_this()});
		return stream.str();
	}
	auto load_entity(Entity_manager& manager, const ETO& eto) -> Entity_ptr {
		std::istringstream stream{eto};
		manager.read(stream, false);
		return manager._entities.back();
	}

} /* namespace ecs */
}
