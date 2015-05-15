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

namespace mo {
	namespace level{class Level;}

namespace sys {
namespace ai {

	class Simple_ai_comp : public ecs::Component<Simple_ai_comp>, public controller::Controller {
		public:
			static constexpr const char* name() {return "Simple_ai";}
			void load(ecs::Entity_state&)override;
			void store(ecs::Entity_state&)override;

			Simple_ai_comp(ecs::Entity& owner);

			void operator()(controller::Controllable_interface&)override;

			void target(ecs::Entity_ptr e)noexcept {
				_target = e;
				_follow_time_left = _follow_time;
			}
			void no_target(Time dt, level::Level& level)noexcept;

			Distance attack_distance;
			Distance near;
			Distance max;
			Angle near_angle;
			Angle far_angle;


			struct Persisted_state;
			friend struct Persisted_state;

		private:
			Time _follow_time;
			Time _follow_time_left;
			ecs::Entity_ptr _target;
			Angle _wander_dir;
			Time _rot_delay;
	};

}
}
}
