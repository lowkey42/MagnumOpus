/**************************************************************************\
 * simple find-and-destroy type ai                                        *
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

#include "../../../core/ecs/ecs.hpp"
#include "../../../core/units.hpp"

#include "../controller/controller.hpp"

namespace game {
namespace sys {
namespace ai {

	class Simple_ai_comp : public core::ecs::Component<Simple_ai_comp>, public controller::Controller {
		public:
			static constexpr const char* name() {return "Simple_ai";}
			void load(core::ecs::Entity_state&)override;
			void store(core::ecs::Entity_state&)override;

			Simple_ai_comp(core::ecs::Entity& owner);

			void operator()(controller::Controllable_interface&)override;

			void target(core::ecs::Entity_ptr e)noexcept {
				_target = e;
				_follow_time_left = _follow_time;
			}
			void no_target(core::Time dt)noexcept;

			core::Distance attack_distance;
			core::Distance near;
			core::Distance max;
			core::Angle near_angle;
			core::Angle far_angle;


			struct Persisted_state;
			friend struct Persisted_state;

		private:
			core::Time _follow_time;
			core::Time _follow_time_left;
			core::ecs::Entity_ptr _target;
	};

}
}
}
