/**************************************************************************\
 * particle system                                                        *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *  Based on code of GDW-SS2014 project by Stefan Bodenschatz             *
 *  which was distributed under the MIT license.                          *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include "../units.hpp"
#include "../utils/maybe.hpp"
#include "../utils/math.hpp"

#include "shader.hpp"
#include "vertex_object.hpp"

namespace mo {
namespace renderer {

	struct Particle {
		glm::vec2 position;
		glm::vec4 color;
		glm::vec2 size;
		float     rotation;

		glm::vec2 velocity;
		float     angular_velocity;
		float     time_to_live;
		float     max_age;

		Particle(glm::vec2 pos, glm::vec4 color, glm::vec2 size, float rotation,
				 glm::vec2 vel, float rot_vel, float ttl);
	};

	struct Environment_callback {
		virtual ~Environment_callback()noexcept{}

		virtual void handle(Position& p, Velocity& vel)const noexcept=0;
	};

	class Particle_emiter {
		public:
			void update_center(Position center);


		// private API
			Particle_emiter(Position center, Distance radius,
			                bool physical, bool aligned,
			                float spawn_rate, std::size_t max_particles,
			                Time min_ttl, Time max_ttl,
			                util::Xerp<Angle> direction,
			                util::Xerp<Speed_per_time> acceleration,
			                util::Xerp<Angle_acceleration> angular_acceleration,
			                util::Xerp<glm::vec4> color,
			                util::Xerp<Position> size);

			void update(bool active, Time dt, Environment_callback& env);

			void draw();

			bool visible(glm::vec2 top_left, glm::vec2 bottom_right)const noexcept;

			bool empty()const noexcept;

		private:
			Position  _center;
			Distance  _radius;
			float     _spawn_rate;
			bool      _physical;
			bool      _aligned;
			Time      _min_ttl;
			Time      _max_ttl;

			util::Xerp<Angle>              _direction;
			util::Xerp<Speed_per_time>     _acceleration;
			util::Xerp<Angle_acceleration> _angular_acceleration;
			util::Xerp<glm::vec4>          _color;
			util::Xerp<Position>           _size;

			glm::vec2             _top_left;
			glm::vec2             _bottom_right;
			std::vector<Particle> _particles;
			Object                _obj;
	};
	using Particle_emiter_ptr = std::shared_ptr<Particle_emiter>;


	class Particle_renderer {
		public:
			Particle_renderer(asset::Asset_manager& assets, std::unique_ptr<Environment_callback> env=std::unique_ptr<Environment_callback>());

			template<typename... Args>
			Particle_emiter_ptr create_emiter(Args&&... args);

			void draw(Time dt, glm::vec2 top_left, glm::vec2 bottom_right);

		private:
			std::unique_ptr<Environment_callback> _env;
			Shader_program _prog;

			std::vector<Particle_emiter_ptr> _emiter;
	};

	template<typename... Args>
	Particle_emiter_ptr Particle_renderer::create_emiter(Args&&... args) {
		auto pe = std::make_shared<Particle_emiter>(std::forward<Args>(args)...);
		_emiter.push_back(pe);

		return pe;
	}
}
}
