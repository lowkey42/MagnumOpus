/**************************************************************************\
 * simple (weighted) pathfinding                                          *
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
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace mo {
namespace util {
	struct position {
		int x, y;
		position(): x(0), y(0){}
		position(int x, int y): x(x), y(y){}
		position(float x, float y): x(static_cast<int>(x+0.5f)), y(static_cast<int>(y+0.5f)){}

		bool operator==(const position& s)const {return x==s.x && y==s.y;}
		bool operator<(const position& s)const {
			if(x==s.x) return y<s.y;
			else       return x<s.x;
		}
	};
	inline auto operator+(const position& a, const position& b) {return position{a.x+b.x, a.y+b.y};}

	typedef std::vector<position> path;

}
}

namespace std {
	template <> struct hash<mo::util::position> {
		size_t operator()(const mo::util::position& p)const noexcept {
			return p.x + p.y*71;
		}
	};
}

namespace mo {
namespace util {

	template<class CostCalculatorType>
	class astar {
		public:
			astar(position limits, CostCalculatorType cost_calculator=CostCalculatorType())
			    : _limits(limits), _cost_calculator(cost_calculator) {}

			path search(position start, position target);

		private:
			struct node {
				position pos;
				float costs;

				int32_t prev_index;
				int32_t path_length;

				node(position pos, float costs, int32_t prev_index, uint16_t path_length)
					: pos(pos), costs(costs), prev_index(prev_index), path_length(path_length) {}

				bool operator>(const node& o)const {
					return costs>o.costs;
				}
			};
			using node_comp = std::greater<node>;

			auto build_path(const node& target_node)const -> path;
			void process_successor(int32_t prev_index, const node& prev_node, position offset, position target);


			std::vector<node> _open_list;

			std::unordered_map<position, std::size_t> _closed_set;
			std::vector<node> _closed_list;

			const position _limits;
			const CostCalculatorType _cost_calculator;
	};

	template<class CostCalculatorType>
	inline auto create_path_finder(position limits, CostCalculatorType cost_calculator=CostCalculatorType()) {
		return astar<CostCalculatorType>(limits, cost_calculator);
	}

	template<class CostCalculatorType>
	auto astar<CostCalculatorType>::search(position start, position target) -> path  {
		_open_list.clear();
		_closed_list.clear();
		_closed_set.clear();

		_open_list.emplace_back(start, 0, -1, 1);
		std::push_heap(_open_list.begin(), _open_list.end(), node_comp());

		int i=0;
		while( !_open_list.empty() ) {
			i++;

			// add to closed list
			auto field_index = int32_t(_closed_list.size());
			_closed_list.push_back(_open_list.front());
			auto& field = _closed_list.back();
			_closed_set[field.pos]=field_index;
			assert( _closed_set.find(field.pos)!=_closed_set.end() );

			// remove from open list
			std::pop_heap(_open_list.begin(), _open_list.end(), node_comp());
			_open_list.pop_back();

			assert(field.costs<=_open_list.front().costs);

			// target reached
			if( field.pos==target )
				return build_path(field);

			for( auto i : {-1,1} ) {
				process_successor( field_index, field, position{i,0}, target );
				process_successor( field_index, field, position{0,i}, target );
			}
		}

		std::cerr<<"A* gave up after "<<i<<" iterations"<<std::endl;

		return path();
	}

	template<class CostCalculatorType>
	path astar<CostCalculatorType>::build_path(const node& targetField)const {
		path path(targetField.path_length-1ul);

		const node* f = &targetField;
		for(auto i=path.size()-1; f->prev_index>=0; i--, f=&_closed_list[f->prev_index] ) {
			assert(i>=0);

			path[i] = f->pos;
		}

		return path;
	}

	template<class CostCalculatorType>
	void astar<CostCalculatorType>::process_successor(int32_t prev_index, const node& prev_node, position offset, position target) {
		const position pos = offset+prev_node.pos;
		const float costs = prev_node.costs + _cost_calculator(
				prev_node.prev_index>=0 ? _closed_list[prev_node.prev_index].pos : prev_node.pos,
						prev_node.pos,
						pos,
						target);

		if( pos.x<=0 || pos.x>=_limits.x-1 || pos.y<=0 || pos.y>=_limits.y-1 || costs<=-1 )
			return;

		if( _closed_set.find(pos)==_closed_set.end() ) {
			for(std::size_t i=0; i<_open_list.size(); ++i) {
				node& n = _open_list[i];

				if( n.pos==pos ) {
					// update score and path
					if( costs<n.costs ) {
						n.costs=costs;
						n.prev_index = prev_index;
						n.path_length = prev_node.path_length+1;

						// reorder heap
						std::make_heap(_open_list.begin(), _open_list.end(), node_comp());
					}

					return;
				}
			}

			// not in open or closed list
			_open_list.emplace_back(pos, costs, prev_index, prev_node.path_length+1);
			std::push_heap(_open_list.begin(), _open_list.end(), node_comp());
		}
	}

}
}
