#include "particles.hpp"

#include "primitives.hpp"
#include "graphics_ctx.hpp"

#include "../utils/random.hpp"

#include <limits>

namespace mo {
namespace renderer {

	using namespace util;
	using namespace unit_literals;

	namespace {
		struct Base_vertex {
			glm::vec2 xy;
			glm::vec2 uv;
			Base_vertex(glm::vec2 xy, glm::vec2 uv) : xy(xy), uv(uv) {}
		};

		Vertex_layout particle_vertex_layout {
			Vertex_layout::Mode::triangle_strip,
			vertex("xy",       &Base_vertex::xy,        0, 0),
			vertex("uv",       &Base_vertex::uv,        0, 0),
			vertex("position", &Particle::position,     1, 1),
			vertex("color",    &Particle::color,        1, 1),
			vertex("size",     &Particle::size,         1, 1),
			vertex("rotation", &Particle::rotation,     1, 1),
			vertex("frame",    &Particle::frame,        1, 1),
			vertex("ttl",      &Particle::time_to_live, 1, 1),
			vertex("max_age",  &Particle::max_age,      1, 1)
		};

		std::vector<Base_vertex> particle_vertices {
			{{-.5,-.5}, {0,1}},
			{{ .5,-.5}, {1,1}},
			{{-.5, .5}, {0,0}},
			{{ .5, .5}, {1,0}}
		};

		struct Default_env_callback : Environment_callback {
			bool check_collision(int, int)noexcept {
				return false;
			}
		};

		auto rng = create_random_generator();

		auto rand_point(float radius) -> glm::vec2 {
			auto phi = random_real(rng, 0.f, 2*PI);
			auto r = random_real(rng, 0.f, radius);
			return {r*glm::cos(phi), r*glm::sin(phi)};
		}

		auto next_pseed() -> uint16_t {
		//	static uint16_t n = 0;
		//	return n++;
			return random_int(rng, uint16_t{0}, std::numeric_limits<uint16_t>::max());
		}
	}

	Particle::Particle(glm::vec2 pos, float rot, float ttl, uint16_t seed)
	    : seed(seed), position(pos), color{0}, size{0}, orientation{rot}, frame{0},
	      velocity{0}, angular_velocity{0}, time_to_live(ttl), max_age(ttl)
	{
	}

	Particle_emiter::Particle_emiter(
	        Position center, Angle orientation, Distance radius,
	        Collision_handler collision_handler,
	        float spawn_rate, std::size_t max_particles,
	        Time min_ttl, Time max_ttl,
	        util::Xerp<Angle> direction,
	        util::Xerp<Angle> rotation_offset,
	        util::Xerp<Speed_per_time> acceleration,
	        util::Xerp<Angle_acceleration> angular_acceleration,
	        util::Xerp<glm::vec4> color,
	        util::Xerp<Position> size,
	        util::Xerp<int8_t> frame,
	        Texture_ptr texture,
	        bool reverse)
	    : _center(center), _orientation(orientation), _radius(radius), _spawn_rate(spawn_rate),
	      _collision_handler(collision_handler),
	      _min_ttl(min_ttl), _max_ttl(max_ttl), _max_particles(max_particles), _reverse(reverse),
	      _frame(frame), _direction(direction), _rotation_offset(rotation_offset),
	      _acceleration(acceleration), _angular_acceleration(angular_acceleration),
	      _color(color), _size(size), _texture(texture),
	      _obj(particle_vertex_layout, create_buffer(particle_vertices),
	           create_dynamic_buffer<Particle>(max_particles))
	{
		_bottom_right = _top_left = remove_units(_center);
	}

	void Particle_emiter::update_center(Position center, Angle orientation) {
		_center = center;
		_orientation = orientation;
	}

	void Particle_emiter::update(bool active, Time dt, Environment_callback& env) {
		if(active && _activated) {
			spawn_new(dt, env);
		}

		auto di = simulate(dt, env);

		_particles.erase(di, _particles.end());
	}
	void Particle_emiter::spawn_new(Time dt, Environment_callback& env) {
		_dt_acc+=dt;

		auto to_spawn = static_cast<std::size_t>(std::round(_spawn_rate * _dt_acc.value()));
		_dt_acc-=Time(to_spawn/_spawn_rate);

		to_spawn = glm::min(to_spawn, _max_particles-_particles.size());

		for(std::size_t i=0; i<to_spawn; ++i) {
			auto seed = next_pseed();

			auto start = remove_units(_center)+rand_point(_radius.value());
			auto dir = _direction(0, seed).value() + _orientation.value();
			auto ttl = random_int(rng, _min_ttl, _max_ttl).value();

			if(_reverse) {
				auto dest = start+rotate(glm::vec2{1,0}, Angle(dir)) * ttl * _acceleration.avg(seed).value() * ttl;

				if(_collision_handler!=Collision_handler::none) {
					auto max_distance = glm::length(dest-start);
					auto step = (dest-start) / max_distance / 2.f;
					auto p=dest + step;
					auto dist=0.5f;
					auto collided = false;
					for(; dist<=max_distance; dist+=0.5, p+=step) {
						auto x = static_cast<int32_t>(p.x+0.5f);
						auto y = static_cast<int32_t>(p.y+0.5f);
						if(env.check_collision(x,y)) {
							collided = true;
							break;
						}
					}
					if(collided)
						continue;
				}

				start = dest;
				dir -= (180.0_deg).value();
			}

			_particles.emplace_back(
				start,
				dir,
				ttl, seed);
		}
	}
	auto Particle_emiter::simulate(Time dt,
	                               Environment_callback& env) -> std::vector<Particle>::iterator {
		if(_particles.empty())
			return _particles.end();

		_bottom_right = _top_left = remove_units(_center);

		auto last = _particles.end() -1;

		for(auto iter=_particles.begin(); iter!=last+1;) {
			if(simulate_one(dt.value(), env, *iter)) {
				++iter;

			} else {
				std::swap(*last, *iter);
				last--;
			}
		}

		return last+1;
	}
	auto Particle_emiter::simulate_one(float dt, Environment_callback& env, Particle& p) -> bool {
		auto t = !_reverse ? (p.max_age-p.time_to_live) / p.max_age
		                   : 1- (p.max_age-p.time_to_live) / p.max_age;

		p.color    = _color(t, p.seed);
		p.size     = remove_units(_size(t, p.seed));
		p.frame    = _frame(t, p.seed);

		p.velocity         += (_acceleration(t, p.seed)*dt).value();
		p.angular_velocity += (_angular_acceleration(t, p.seed)*dt).value();

		p.orientation += p.angular_velocity*dt;
		p.rotation = p.orientation + _rotation_offset(t, p.seed).value() - 90_deg;

		if(_collision_handler!=Collision_handler::none) {
			int x = static_cast<int>(p.position.x+0.5);
			int y = static_cast<int>(p.position.y+0.5);

			if(env.check_collision(x,y)) {
				if(_collision_handler==Collision_handler::kill)
					return false;


				auto rel = glm::vec2{x-p.position.x, y-p.position.y};
				auto arel = glm::abs(rel);

				if((arel.x<0.01 && arel.y<0.01))
					return false;

				auto dv = glm::vec2{glm::cos(p.orientation), glm::sin(p.orientation)};

				if(glm::abs(arel.x-arel.y) < 0.01) {
					dv.x=-dv.x;
					dv.y=-dv.y;

				} else if(arel.x > arel.y) {
					dv.x=-dv.x;
				} else {
					dv.y=-dv.y;
				}

				auto np = p.position+ dv*p.velocity * (1/30.f);
				if(env.check_collision(np.x+0.5f, y+0.5f))
					return false;

				p.orientation = glm::atan(dv.y, dv.x);

				switch(_collision_handler) {
					case Collision_handler::stop:
						p.velocity*=0.2f;

					case Collision_handler::bounce:
					default:
						p.velocity*=0.9f;
						break;
				}
			}
		}

		auto r = p.velocity * dt;
		p.position += r*glm::vec2{glm::cos(p.orientation), glm::sin(p.orientation)};

		update_bounds(p);

		p.time_to_live=glm::max(p.time_to_live-dt, 0.f);
		return p.time_to_live>0;
	}
	void Particle_emiter::update_bounds(const Particle& p) {
		if(p.position.x<_top_left.x)
			_top_left.x = p.position.x;

		if(p.position.y<_top_left.y)
			_top_left.y = p.position.y;

		if(p.position.x>_bottom_right.x)
			_bottom_right.x = p.position.x;

		if(p.position.y>_bottom_right.y)
			_bottom_right.y = p.position.y;
	}

	void Particle_emiter::draw(Shader_program& prog) {
		if(_texture)
			_texture->bind();

		prog.set_uniform("frames", static_cast<float>(_frame.max()+1));

		_obj.buffer(1).set(_particles);
		_obj.draw();
	}
	bool Particle_emiter::visible(glm::vec2 top_left, glm::vec2 bottom_right)const noexcept {
		return !(
		            top_left.x     > _bottom_right.x ||
		            bottom_right.x < _top_left.x ||
		            top_left.y     > _bottom_right.y ||
		            bottom_right.y < _top_left.y
		        );
	}
	bool Particle_emiter::empty()const noexcept {
		return _particles.empty();
	}


	Particle_renderer::Particle_renderer(asset::Asset_manager& assets,
	                                     std::unique_ptr<Environment_callback> env)
	    : _env(env ? std::move(env) : std::make_unique<Default_env_callback>())
	{
		_prog.attach_shader(assets.load<renderer::Shader>("vert_shader:particles"_aid))
		     .attach_shader(assets.load<renderer::Shader>("frag_shader:particles"_aid))
		     .bind_all_attribute_locations(renderer::particle_vertex_layout)
		     .build();
	}

	void Particle_renderer::draw(Time dt, const Camera& cam) {
		Disable_depthwrite ddw{};

		auto cam_area  = cam.area();
		glm::vec2 top_left    {cam_area.x, cam_area.y};
		glm::vec2 bottom_right{cam_area.z, cam_area.w};

		_prog.bind()
		     .set_uniform("vp", cam.vp())
		     .set_uniform("texture", 0)
		     .set_uniform("layer", 0.9f);

		for(auto& pe : _emiter) {
			if(pe->visible(top_left, bottom_right)) {
				pe->update(pe.use_count()>1, dt, *_env);
				pe->draw(_prog);
			}
		}

		_prog.unbind();

		_emiter.erase(
		            std::remove_if(_emiter.begin(),
		                           _emiter.end(),
		                           [](auto& pe){return pe.use_count()<=1 && pe->empty();}),
		            _emiter.end());
	}

}
}
