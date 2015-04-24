/**************************************************************************\
 * human-created rooms                                                    *
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

#include <vector>
#include <iostream>

#include "elements.hpp"
#include "level.hpp"

#include "../../core/asset/asset_manager.hpp"

namespace game {
namespace level {

	enum class Room_obj_type {
		// TODO
	};

	struct Room_tile_props {
		bool doorAllowed;
	};
	struct Room_object_props {
		float x, y;
		Room_obj_type type;
	};

	// currently played part of the world
	class Room_template : public Level {
		public:
			Room_template() = default;
			Room_template(Tile_type default_type, int width, int height);
			Room_template(int width, int height, std::vector<Tile> data, std::vector<Room_tile_props> props, std::vector<Room_object_props> objs);

			auto& get_prop(int x, int y) {return _props[y*_width + x];}
			auto  get_objects()const {return core::util::range(_objs);}

		private:
			virtual void _store(TiledLevel& l)const;
			virtual void _load(const TiledLevel& l);

			std::vector<Room_tile_props> _props;
			std::vector<Room_object_props> _objs;
	};

}
}

namespace core {
namespace asset {
	template<>
	struct Loader<game::level::Room_template> {
		using RT = std::shared_ptr<const game::level::Room_template>;

		RT operator()(istream in) throw(Loading_failed){
			auto r = std::make_shared<game::level::Room_template>();
			r->load(in);
			return r;
		}

		void operator()(ostream out, const game::level::Room_template& asset) throw(Loading_failed) {
			asset.store(out);
		}
	};
}
}
