
#include "controller_system.hpp"

#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"
#include "../combat/comp/weapon_comp.hpp"
#include "../state/state_comp.hpp"
#include "../item/collector_comp.hpp"
#include "../item/element_comp.hpp"

namespace mo {
namespace sys {
namespace controller {
	using namespace physics;
	using namespace state;
	using namespace unit_literals;

	Controller_system::Controller_system(ecs::Entity_manager& entity_manager)
	  : _controllables(entity_manager.list<Controllable_comp>()),
	    _on_comp_events(&Controller_system::_on_comp_event, this) {

		entity_manager.register_component_type<Controllable_comp>();
		_on_comp_events.connect(entity_manager.list<Controllable_comp>());
	}

	void Controller_system::_on_comp_event(ecs::Component_event e) {
		if(e.type==ecs::Component_event_type::freed) {
			e.handle.get<Controllable_comp>().process([&](auto& cont){
				if(cont._controller)
					cont._controller->request_unjoin();
			});
		}
	}

	namespace {

		struct Controllable_interface_impl : public Controllable_interface {
			Controllable_interface_impl(Time dt, ecs::Entity& entity, bool& active)
				: _dt(dt), _entity(entity),
				  _state(entity.get<State_comp>().process(State_data{},
														   [](auto& s){return s.state();})),
				  _target_rotation(entity.get<Transform_comp>().process(0_deg,
																	[](auto& p){return p.rotation();})),
				  _active(active) {}

			~Controllable_interface_impl()noexcept;

			void move(glm::vec2 target) override;
			void look_at(glm::vec2 pos) override;
			void look_in_dir(glm::vec2 direction) override;
			void attack() override;
			void use() override;
			void take() override;
			void switch_weapon(uint32_t weapon_id) override;

			void set_state(Entity_state s, float magnitude=1.f) {
				_entity.get<State_comp>().process([s,magnitude](auto& state){
					state.state(s, magnitude);
				});
			}

			private:
				const Time _dt;
				ecs::Entity& _entity;
				const State_data _state;
				Angle _target_rotation;
				bool& _active;
		};
	}
	void Controller_system::update(Time dt) {
		for(auto& controllable : _controllables) {
			controllable._active = false;

			Controllable_interface_impl c(dt, controllable.owner(), controllable._active);

			if(controllable._controller)
				(*controllable._controller)(c);

			else if(controllable._controller_component)
				controllable.owner().getByType<Controller>(controllable._controller_component)
						.process([&c](auto& controller){
					controller(c);
				} );
		}
	}

	void Controller_system::feedback(float force) {
		for(auto& controllable : _controllables) {
			if(controllable._controller) {
				controllable._controller->feedback(force);
			}
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

	auto Controller_manager::gamepad(std::size_t idx, bool activate) -> util::maybe<Controller&> {
		auto ret = idx<_active_gamepad_controller.size()
			  ? util::justPtr<Controller>(_active_gamepad_controller[idx].get())
			  : util::nothing();

		if(ret.is_nothing() && activate && idx<_ready_gamepad_controller.size()+_active_gamepad_controller.size()) {
			auto missing = idx-_active_gamepad_controller.size();
			for(std::size_t i=0; i<=missing; i++)
				_ready_gamepad_controller.back()->request_join();

			if(idx<_active_gamepad_controller.size())
				ret = util::justPtr<Controller>(_active_gamepad_controller[idx].get());
		}

		return ret;
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

		void Controllable_interface_impl::move(glm::vec2 direction) {
			_entity.get<Physics_comp>().process([this, &direction](auto& comp){
				auto len = glm::length(direction);
				if(len>1) {
					direction/=len;
					len = 1;
                }else if(len<0.00001 || std::isnan(len))
					return;

				_active = true;

				comp.accelerate_active(direction);

				if(_state.s!=Entity_state::attacking_melee && _state.s!=Entity_state::attacking_range)
					this->look_in_dir(direction);

				this->set_state(Entity_state::walking, len);
			});
		}
		void Controllable_interface_impl::look_at(glm::vec2 pos) {
			_entity.get<Transform_comp>().process([this, pos](auto& comp){
				this->look_in_dir(pos-remove_units(comp.position()));
			});
		}
		void Controllable_interface_impl::look_in_dir(glm::vec2 direction) {
			_target_rotation = Angle(atan2(direction.y, direction.x));
		}
		void Controllable_interface_impl::attack() {
			_entity.get<combat::Weapon_comp>().process([&](auto& w){
				w.attack();
				_active = true;
			});
		}
		void Controllable_interface_impl::use() {
			// TODO: UserComponent.use(map.get(TransformComp.getPosition(), TransformComp.getRotation()))

			// if there is something useable
			set_state(Entity_state::interacting);
		}
		void Controllable_interface_impl::take() {
			// TODO: InventoryComponent.take(map.get(TransformComp.getPosition(), TransformComp.getRotation()))

			_entity.get<item::Collector_comp>().process([&](auto& c){
				c.take();
				_active = true;
			});

			set_state(Entity_state::taking);
		}
		void Controllable_interface_impl::switch_weapon(uint32_t weapon_id) {
			auto r = _entity.get<item::Element_comp>().process(false, [&](item::Element_comp& e){
				_active = true;
				return e.flip_slot(weapon_id);
			});

			if(r) {
				set_state(Entity_state::change_weapon);
			}
		}

		Controllable_interface_impl::~Controllable_interface_impl()noexcept {
			_entity.get<Transform_comp>().process([&](auto& c){
				if(std::abs(normalize_to_half_rot(_target_rotation-c.rotation()).value()) > (20_deg).value())
					_active = true;

				c.rotate( normalize_to_half_rot(_target_rotation-c.rotation()), _dt );
			});
		}
	}

}
}
}
