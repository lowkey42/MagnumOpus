#include "level_generator.hpp"

#include "room_template.hpp"

#include "../../core/utils/astar.hpp"
#include "../../core/utils/string_utils.hpp"
#include <core/utils/random.hpp>

#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#include <algorithm>
#include <random>
#include <glm/glm.hpp>

using namespace mo::util;

namespace {
	template<typename T>
	struct Range {
		T min;
		T max;
	};
	using IntRange = Range<int>;
	sf2_structDef(IntRange, sf2_member(min), sf2_member(max))


	struct Dungeon_cfg {
		IntRange room_size;
		IntRange rooms;
		int max_width, max_height;
		float split_prop_factor;
	};
	sf2_structDef(Dungeon_cfg,
		sf2_member(room_size),
		sf2_member(rooms),
		sf2_member(max_width),
		sf2_member(max_height),
		sf2_member(split_prop_factor)
	)

	struct Dungeon_cfg_map {
		std::unordered_map<int, Dungeon_cfg> levels;
	};
	sf2_structDef(Dungeon_cfg_map, sf2_member(levels))
}

namespace mo {
namespace asset {

	template<>
	struct Loader<Dungeon_cfg_map> {
		using T = Dungeon_cfg_map;
		static auto load(istream in) throw(Loading_failed) -> std::shared_ptr<T> {
			auto i = std::make_shared<T>();
			sf2::parseStream(in, *i);
			return i;
		}
		static void store(ostream out, T& asset) throw(Loading_failed);
	};

}

namespace level {
	namespace {
		Dungeon_cfg load_cfg(asset::Asset_manager& assets, int depth) {
			auto opts = assets.load<Dungeon_cfg_map>("cfg:dungeons"_aid);
			auto cfg_iter = opts->levels.find(depth);
			INVARIANT(cfg_iter!=opts->levels.end(), "You delved too greedily and too deep. Level="+to_string(depth));

			return cfg_iter->second;
		}

		struct Room_blueprint : public Room {
			std::vector<std::size_t> connections;
			asset::Ptr<Room_template> room_template;

			Room_blueprint(int top, int left, int right, int bottom)
				: Room(top, left, right, bottom) {}

			Room_blueprint split(int new_width, int new_height) {
				Room_blueprint r {
					top + new_height,
					left + new_width,
					right,
					bottom
				};

				if(new_width>0)
					right = left + new_width;

				if(new_height>0)
					bottom = top + new_height;

				return r;
			}
		};

		template<class T>
		std::vector<T> join(std::vector<T>&& a, std::vector<T>&& b) {
			a.reserve( a.size() + b.size() );
			std::move(b.begin(), b.end(), std::back_inserter(a));
			return a;
		}

		using Room_list = std::vector<Room_blueprint>;

		auto process_room(int depth, Room_blueprint room,
		                  random_generator& rng, const Dungeon_cfg& cfg) -> Room_list;
		auto filter_rooms(Room_list rooms, random_generator& rng, const Dungeon_cfg& cfg) -> Room_list;
		void connect_rooms(Room_list& rooms, random_generator& rng, const Dungeon_cfg& cfg);
		auto build_level(Room_list& rooms, int width, int height) -> Level;
		void dig_corridors(Level& level, const Room_list& rooms);
		void decorate_rooms(Level& level, const Room_list& rooms);
	}

	Level generate_level(asset::Asset_manager& assets, uint64_t seed,
	                     int depth, int difficulty) {
		const auto cfg = load_cfg(assets, depth);

		auto rng = random_generator{seed+depth*7+difficulty*31};


		// 1. map zufällig an größter Achse trennen (rekursiv für die beiden neuen Zellen wiederholen)
		auto rooms = process_room(1, Room_blueprint{0,0, cfg.max_width, cfg.max_height}, rng, cfg);


		// 2. zufällig Zellen verwerfen bis nur noch N übrig
		rooms = filter_rooms(rooms, rng, cfg);

		// 3. Graph/ Pfad aus den Zellen bauen (minimum-spanning tree)
		connect_rooms(rooms, rng, cfg);

		// 4. Rollenzuweisung anhand des Pfades

		// 5. Templates anhand der Rollen zuweisen

		// 6. Räume in Level konvertieren
		Level level = build_level(rooms, cfg.max_width, cfg.max_height);

		// 7. Tunnel für vorgesehene Pfade graben
		dig_corridors(level, rooms);

		// 8. Räume "befüllen"
		decorate_rooms(level, rooms);

		// 9. Objekte erstellen



		return level;
	}

	namespace {

		auto process_room(int depth, Room_blueprint room,
									  random_generator& rng, const Dungeon_cfg& cfg) -> Room_list {
			bool split = (room.height()>=cfg.room_size.min || room.width()>=cfg.room_size.min) &&
						 (room.height()>cfg.room_size.max  || room.width()>cfg.room_size.max ||
							random_bool(rng, std::min(std::max(cfg.split_prop_factor/depth, 0.1f), 1.f)));

			if(split) {
				bool split_x = room.width() >= room.height();

				auto b = room.split(
					!split_x ? 0 : random_int(rng, 1, room.width() -1),
					split_x  ? 0 : random_int(rng, 1, room.height()-1)
				);

				return join(
					process_room(depth+1, room, rng, cfg),
					process_room(depth+1, b, rng, cfg)
				);

			} else {
				return {room};
			}
		}

		auto filter_rooms(Room_list rooms, random_generator& rng, const Dungeon_cfg& cfg) -> Room_list {
			rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [&](auto& r){
				return r.height()<=cfg.room_size.min || r.width()<cfg.room_size.min;
			}), rooms.end());

			auto target_room_count = random_int<std::size_t>(rng, cfg.rooms.min, cfg.rooms.max);

			while(rooms.size()>target_room_count) {
                auto i = random_int<std::size_t>(rng, 0, rooms.size()-1);
                if(i<rooms.size()-1)
                    rooms[i] = rooms.back();

				rooms.pop_back();
			}

			return rooms;
		}


		template<class T, class Func>
		std::size_t find_min(std::vector<T>& c, Func weight_func) {
			assert(!c.empty());

			if(c.size()==1)
				return 0;

			std::size_t min = 0;
			auto weight = weight_func(c[0]);
			for(std::size_t i=1; i<c.size(); ++i) {
				auto nw = weight_func(c[i]);
				if(nw<weight) {
					min = i;
					weight = nw;
				}
			}

			return min;
		}
		template<class T>
		T fast_erase(std::vector<T>& c, std::size_t i) {
			T tmp = c[i];
			c[i] = c.back();
			c.pop_back();
			return tmp;
		}

		template<class T, class Func>
		T pop_min(std::vector<T>& c, Func weight_func) {
			return fast_erase(c, find_min(c, weight_func));
		}
		template<class T>
		T pop_random(std::vector<T>& c, random_generator& rng) {
			return fast_erase(c, random_int<std::size_t>(rng, 0, c.size()-1));
		}

		void connect_rooms(Room_list& rooms, random_generator& rng, const Dungeon_cfg& cfg) {
			std::vector<std::size_t> open_list(rooms.size());
			std::iota(open_list.begin(), open_list.end(), 0);

			auto last_room = pop_random(open_list, rng);
			rooms.at(last_room).type = Room_type::start;


			const auto closest_room_w = [&last_room, &rooms](std::size_t rIdx){
				auto dist_x = std::abs(rooms.at(rIdx).center().x-rooms.at(last_room).center().x);
				auto dist_y = std::abs(rooms.at(rIdx).center().y-rooms.at(last_room).center().y);

				if( std::abs(dist_x-dist_y)<10 ) // TODO: impl better heuristic
					return std::min(dist_x, dist_y);
				else
					return dist_x + dist_y;
			};

			while(!open_list.empty()) {
				auto closest = pop_min(open_list, closest_room_w);

				rooms.at(last_room).connections.push_back(closest);

				last_room = closest;
			}

			rooms.at(last_room).type = Room_type::end;
		}

		Level build_level(Room_list& rooms, int width, int height) {
			constexpr int border = 5;
			int min_x = std::numeric_limits<int>::max();
			int min_y = std::numeric_limits<int>::max();
			int max_x = std::numeric_limits<int>::min();
			int max_y = std::numeric_limits<int>::min();

			for(Room& r : rooms) {
				min_y = std::min(min_y, r.top);
				max_y = std::max(max_y, r.bottom);
				min_x = std::min(min_x, r.left);
				max_x = std::max(max_x, r.right);
			}

			int level_width  = max_x-min_x+border*2;
			int level_height = max_y-min_y+border*2;

			auto level_rooms = std::vector<Room>();
			level_rooms.reserve(rooms.size());
			int rid=0;
			for(auto& r : rooms) {
				level_rooms.emplace_back(
				            r.top - min_y +border,
				            r.left- min_x +border,
				            r.right - min_x +border,
				            r.bottom - min_y +border,
				            r.type,
				            rid++
				);
			}

			Level level(Tile_type::wall_dirt, level_width, level_height, level_rooms);

			for(auto& r : rooms) {
				for(int x=0; x<r.width(); ++x) {
					auto ex = border+x+r.left-min_x;

					level.get(ex, border+r.top-min_y).type = Tile_type::wall_tile;
					level.get(ex, border+r.top-min_y+r.height()-1).type = Tile_type::wall_tile;

					for(int y=1; y<r.height()-1; ++y) {
						if(x==0 || x==r.width()-1)
							level.get(ex, border+y +r.top-min_y).type = Tile_type::wall_tile;
						else
							level.get(ex, border+y +r.top-min_y).type = Tile_type::floor_tile;
					}
				}
				r.left-=min_x-border;
				r.top-=min_y-border;
				r.right-=min_x-border;
				r.bottom-=min_y-border;
			}

			return level;
		}

		void dig_path(Level& level, util::path&& path) {
			for(const auto& node : path) {
				auto& tile = level.get(node.x, node.y);

				if(tile.solid())
					tile.type = Tile_type::floor_tile; // TODO: use doors, etc. from template

				for(int y=std::max(node.y-1, 0); y<=std::min(node.y+1, level.height()-1); ++y)
					for(int x=std::max(node.x-1, 0); x<=std::min(node.x+1, level.width()-1); ++x) {
						auto& btile = level.get(x, y);
						if(   btile.type==Tile_type::indestructible_wall
						   || btile.type==Tile_type::wall_dirt
						   || btile.type==Tile_type::wall_stone
						   || btile.type==Tile_type::wall_tile )
							btile.type = Tile_type::wall_tile;
					}
			}
		}

		void dig_corridors(Level& level, const Room_list& rooms) {
			using namespace util;

			auto path_scorer = [&](position pprev, position prev, position node, position goal) {
				auto costs = std::sqrt((node.x-goal.x)*(node.x-goal.x) + (node.y-goal.y)*(node.y-goal.y));

				if(pprev.x!=node.x && pprev.y!=node.y)
					costs+=5;

				if(level.get(node.x, node.y).solid())
					costs+=100;

				return costs;
			};

			auto path_finder = create_path_finder({level.width(), level.height()}, path_scorer);

			for(auto& room : rooms) {
				for(auto c : room.connections) {
					dig_path(level, path_finder.search(
							{room.center().x, room.center().y},
							{rooms[c].center().x, rooms[c].center().y} ));
				}
			}
		}

		void decorate_rooms(Level& level, const Room_list& rooms) {
			for(auto& r : rooms) {
				auto center = r.center();

				switch(r.type) {
					case Room_type::start:
						level.get(center.x, center.y).type = Tile_type::stairs_up;
						break;

					case Room_type::end:
						level.get(center.x, center.y).type = Tile_type::stairs_down;
						break;

					case Room_type::boss:
						break;

					case Room_type::normal:
						break;

					case Room_type::secret:
						break;
				}
			}
		}

	}
}
}
