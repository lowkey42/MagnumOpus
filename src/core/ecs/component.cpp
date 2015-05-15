#include "component.hpp"

#include "ecs.hpp"

namespace mo {
namespace ecs {
namespace details {

	Component_base::Component_base(Entity& owner)noexcept : _owner(&owner) {}

	Component_base::Component_base(Component_base && o)noexcept : _owner(o._owner) {
		o._owner = nullptr;
	}
	Component_base& Component_base::operator=(Component_base&& o)noexcept {
		_owner = o._owner;
		o._owner = nullptr;
		return *this;
	}

	Entity_ptr Component_base::owner_ptr() const {
		return get_entity(*_owner);
	}

	void Component_base::_reg_self(Component_type type) {
		if(_owner) {
			get_component(*_owner, type) = this;
		}
	}

	void Component_base::_unreg_self(Component_type type) {
		if(_owner) {
			get_component(*_owner, type) = nullptr;
		}
	}

	Component_type Component_base::_next_type_id()noexcept {
		static Component_type type_counter = 1;

		auto id = type_counter;
		INVARIANT(id < max_comp_type, "type_counter is to small. Requires at least " << id);
		type_counter++;

		if(type_counter == 0)
			type_counter = 1;

		return id;
	}

}
}
}
