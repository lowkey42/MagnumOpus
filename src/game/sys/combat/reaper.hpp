/**************************************************************************\
 * Collects the dying entities and kills them off                         *
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

#include "../state/state_system.hpp"

namespace mo {
namespace sys {
namespace combat {

	class Reaper {
		public:
			Reaper(ecs::Entity_manager& entity_manager,
				   state::State_system& state_system);

		private:
			void _reap(ecs::Entity&, sys::state::State_data&);

			ecs::Entity_manager& _em;
			util::slot<ecs::Entity&, sys::state::State_data&> _reap_slot;
	};

}
}
}
