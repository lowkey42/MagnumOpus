
#include "controller_system.hpp"

#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

namespace mo {
namespace sys {
namespace controller {
	using namespace physics;
	using namespace unit_literals;

	Controller_system::Controller_system(ecs::Entity_manager& entity_manager)
	  : _controllables(entity_manager.list<Controllable_comp>()),
		_states(entity_manager.list<State_comp>()) {
		entity_manager.register_component_type<Controllable_comp>();
		entity_manager.register_component_type<State_comp>();
	}

	namespace {
		constexpr auto max_rotation_speed = 180_deg / second;

		struct Controllable_interface_impl : public Controllable_interface {
			Controllable_interface_impl(Time dt, ecs::Entity& entity)
				: _dt(dt), _entity(entity),
				  _state(entity.get<State_comp>().process(Entity_state::idle,
														   [](auto& s){return s.state();})) {}

			~Controllable_interface_impl()noexcept;

			auto entity()noexcept -> ecs::Entity& override;
			void move(glm::vec2 target) override;
			void look_at(glm::vec2 pos) override;
			void look_in_dir(glm::vec2 direction) override;
			void attack() override;
			void use() override;
			void take() override;
			void switch_weapon(uint32_t weapon_id) override;

			private:
				const Time _dt;
				ecs::Entity& _entity;
				Entity_state _state;
		};
	}
	void Controller_system::update(Time dt) {
		for(auto& state : _states) {
			state.update(dt);
		}

		for(auto& controllable : _controllables) {
			Controllable_interface_impl c(dt, controllable.owner());

			if(controllable.controller)
				(*controllable.controller)(c);
		}
	}


	Controller_manager::Controller_manager(
			asset::Asset_manager& assets,
			Input_manager& input)
		: _screen_to_world_coords([](glm::vec2 v){return v;}),
		  _mapping(assets.load<Mapping>("cfg:controlls"_aid)),
		  _on_joined(&Controller_manager::_join, this),
		  _on_unjoined(&Controller_manager::_unjoin, this),
		  _on_added(&Controller_manager::_add_gamepad, this),
		  _on_removed(&Controller_manager::_remove_gamepad, this) {

		_on_added.connect(input.gamepad_added_events);
		_on_removed.connect(input.gamepad_removed_events);

		_on_joined.connect(join_events);
		_on_unjoined.connect(unjoin_events);

		_keyboard_controller = std::make_unique<Keyboard_controller>(
			_mapping, quit_events, _screen_to_world_coords,
			input.keyboard_events,
			input.mouse_events,
			input.button_events );

		_main_controller = std::make_unique<Combined_controller>(
			*_keyboard_controller,
			_ready_gamepad_controller );
	}

	void Controller_manager::_join(Controller_added_event e) {
		auto iter = std::find_if(_ready_gamepad_controller.begin(), _ready_gamepad_controller.end(),
			[e](std::unique_ptr<Gamepad_controller>& gp) {
				return gp.get()==&e.controller;
		});

		if(iter!=_ready_gamepad_controller.end()) {
			_active_gamepad_controller.emplace_back(std::move(*iter));
			_ready_gamepad_controller.erase(iter);
		}
	}

	void Controller_manager::_unjoin(Controller_removed_event e) {
		auto iter = std::find_if(_active_gamepad_controller.begin(), _active_gamepad_controller.end(),
			[e](std::unique_ptr<Gamepad_controller>& gp) {
				return gp.get()==&e.controller;
		});

		if(iter!=_active_gamepad_controller.end()) {
			_ready_gamepad_controller.emplace_back(std::move(*iter));
			_active_gamepad_controller.erase(iter);
		}
	}

	void Controller_manager::_add_gamepad(SDL_GameController* c) {
		_ready_gamepad_controller.emplace_back(std::make_unique<Gamepad_controller>(
			_mapping, quit_events, c, join_events, unjoin_events
		));
	}

	void Controller_manager::_remove_gamepad(SDL_GameController* c) {
		auto instId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c));

		erase_if(_active_gamepad_controller, [instId](auto& v){
			return v->instance_id()==instId;
		});
		erase_if(_ready_gamepad_controller, [instId](auto& v){
			return v->instance_id()==instId;
		});
	}

	void Controller_manager::update(Time) {
		_keyboard_controller->on_frame();

		for(auto& c : _active_gamepad_controller)
			c->on_frame();

		for(auto& c : _ready_gamepad_controller)
			c->on_frame();
	}

	namespace {
		auto Controllable_interface_impl::entity()noexcept -> ecs::Entity& {
			return _entity;
		}
		void Controllable_interface_impl::move(glm::vec2 direction) {
			_entity.get<Physics_comp>().process([this, &direction](auto& comp){
				comp.accelerate_active(direction);

				if(_state!=Entity_state::attacking_melee && _state!=Entity_state::attacking_range)
					this->look_in_dir(direction);

				if(_state==Entity_state::idle)
					_state = Entity_state::walking;
			});
		}
		void Controllable_interface_impl::look_at(glm::vec2 pos) {
			_entity.get<Transform_comp>().process([this, pos](auto& comp){
				this->look_in_dir(remove_units(comp.position()) - pos);
			});
		}
		void Controllable_interface_impl::look_in_dir(glm::vec2 direction) {
			_entity.get<Transform_comp>().process([this, direction](auto& comp){
				Angle target_rot = Angle(glm::atan(direction.y, direction.x));
				Angle rotation_diff = target_rot-comp.rotation();
				if(rotation_diff>180_deg) rotation_diff-=360_deg;
				if(rotation_diff<180_deg) rotation_diff+=360_deg;

				comp.rotation(comp.rotation() + std::min(rotation_diff, max_rotation_speed*_dt) );
			});
		}
		void Controllable_interface_impl::attack() {
			// TODO: AttackerComponent.attack(TransformComp.getPosition(), TransformComp.getRotation())

			_state = Entity_state::attacking_range; // TODO: melee vs ranged
		}
		void Controllable_interface_impl::use() {
			// TODO: UserComponent.use(map.get(TransformComp.getPosition(), TransformComp.getRotation()))

			if(_state!=Entity_state::taking)
				_state = Entity_state::interacting;
		}
		void Controllable_interface_impl::take() {
			// TODO: InventoryComponent.take(map.get(TransformComp.getPosition(), TransformComp.getRotation()))

			_state = Entity_state::taking;
		}
		void Controllable_interface_impl::switch_weapon(uint32_t weapon_id) {
			// TODO: WeaponComponent.set(weaponId)

			_state = Entity_state::change_weapon;
		}

		Controllable_interface_impl::~Controllable_interface_impl()noexcept {
			_entity.get<State_comp>().process([&](auto& s){s.state(_state);});
		}
	}

}
}
}
