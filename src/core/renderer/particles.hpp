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
#include "texture.hpp"
#include "camera.hpp"

namespace mo {
namespace renderer {

	struct Particle {
		uint16_t  seed;

		glm::vec2 position;
		glm::vec4 color;
		glm::vec2 size;
		float     rotation;
		float     frame;

		float     velocity;
		float     angular_velocity;
		float     time_to_live;
		float     max_age;

		Particle(glm::vec2 pos, float rot, float ttl, uint16_t seed);
	};

	struct Environment_callback {
		virtual ~Environment_callback()noexcept{}

		virtual bool handle(glm::vec2 p, float& vel, float& dir)noexcept=0;
	};

	class Particle_emiter {
		public:
			void update_center(Position center, Angle orientation);


		// private API
			Particle_emiter(Position center, Angle orientation, Distance radius,
			                bool physical,
			                float spawn_rate, std::size_t max_particles,
			                Time min_ttl, Time max_ttl,
			                util::Xerp<Angle> direction,
			                util::Xerp<Speed_per_time> acceleration,
			                util::Xerp<Angle_acceleration> angular_acceleration,
			                util::Xerp<glm::vec4> color,
			                util::Xerp<Position> size,
			                util::Xerp<int8_t> frame,
			                Texture_ptr texture);

			void update(bool active, Time dt, Environment_callback& env);

			void draw(Shader_program& prog);

			bool visible(glm::vec2 top_left, glm::vec2 bottom_right)const noexcept;

			bool empty()const noexcept;

		private:
			void spawn_new(Time dt);
			auto simulate(Time dt, Environment_callback& env) -> std::vector<Particle>::iterator;
			auto simulate_one(float dt, Environment_callback& env, Particle& p) -> bool;
			void update_bounds(const Particle& p);

			Position    _center;
			Angle       _orientation;
			Distance    _radius;
			float       _spawn_rate;
			bool        _physical;
			Time        _min_ttl;
			Time        _max_ttl;
			std::size_t _max_particles;

			util::Xerp<int8_t>             _frame;
			util::Xerp<Angle>              _direction;
			util::Xerp<Speed_per_time>     _acceleration;
			util::Xerp<Angle_acceleration> _angular_acceleration;
			util::Xerp<glm::vec4>          _color;
			util::Xerp<Position>           _size;

			glm::vec2             _top_left;
			glm::vec2             _bottom_right;
			std::vector<Particle> _particles;
			Texture_ptr           _texture;
			Object                _obj;

			Time _dt_acc {0};
	};
	using Particle_emiter_ptr = std::shared_ptr<Particle_emiter>;


	class Particle_renderer {
		public:
			Particle_renderer(asset::Asset_manager& assets, std::unique_ptr<Environment_callback> env=std::unique_ptr<Environment_callback>());

			Particle_emiter_ptr create_emiter(Position center, Angle orientation, Distance radius,
			                                  bool physical,
							                  float spawn_rate, std::size_t max_particles,
							                  Time min_ttl, Time max_ttl,
							                  util::Xerp<Angle> direction,
							                  util::Xerp<Speed_per_time> acceleration,
							                  util::Xerp<Angle_acceleration> angular_acceleration,
							                  util::Xerp<glm::vec4> color,
							                  util::Xerp<Position> size,
							                  util::Xerp<int8_t> frame,
							                  Texture_ptr texture);

			void draw(Time dt, const Camera& cam);

		private:
			std::unique_ptr<Environment_callback> _env;
			Shader_program _prog;

			std::vector<Particle_emiter_ptr> _emiter;
	};

	inline Particle_emiter_ptr Particle_renderer::create_emiter(Position center, Angle orientation,
	                                                            Distance radius,
	                                                     bool physical,
										                 float spawn_rate, std::size_t max_particles,
										                 Time min_ttl, Time max_ttl,
										                 util::Xerp<Angle> direction,
										                 util::Xerp<Speed_per_time> acceleration,
										                 util::Xerp<Angle_acceleration> angular_acceleration,
										                 util::Xerp<glm::vec4> color,
										                 util::Xerp<Position> size,
										                 util::Xerp<int8_t> frame,
										                 Texture_ptr texture) {
		auto pe = std::make_shared<Particle_emiter>(center, orientation, radius, physical, spawn_rate, max_particles, min_ttl, max_ttl, direction, acceleration, angular_acceleration, color, size, frame, texture);
		_emiter.emplace_back(pe);

		return pe;
	}
}
}
