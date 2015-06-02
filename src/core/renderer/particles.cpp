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
			{{-.5,-.5}, {0,0}},
			{{ .5,-.5}, {1,0}},
			{{-.5, .5}, {0,1}},
			{{ .5, .5}, {1,1}}
		};

		struct Default_env_callback : Environment_callback {
			bool handle(glm::vec2, float&, float&)noexcept {
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
	    : seed(seed), position(pos), color{0}, size{0}, rotation{rot}, frame{0},
	      velocity{0}, angular_velocity{0}, time_to_live(ttl), max_age(ttl)
	{
	}

	Particle_emiter::Particle_emiter(
	        Position center, Distance radius,
	        bool physical,
	        float spawn_rate, std::size_t max_particles,
	        Time min_ttl, Time max_ttl,
	        util::Xerp<Angle> direction,
	        util::Xerp<Speed_per_time> acceleration,
	        util::Xerp<Angle_acceleration> angular_acceleration,
	        util::Xerp<glm::vec4> color,
	        util::Xerp<Position> size,
	        util::Xerp<int8_t> frame,
	        Texture_ptr texture)
	    : _center(center), _radius(radius), _spawn_rate(spawn_rate),
	      _physical(physical),
	      _min_ttl(min_ttl), _max_ttl(max_ttl), _max_particles(max_particles),
	      _frame(frame), _direction(direction),
	      _acceleration(acceleration), _angular_acceleration(angular_acceleration),
	      _color(color), _size(size), _texture(texture),
	      _obj(particle_vertex_layout, create_buffer(particle_vertices),
	           create_dynamic_buffer<Particle>(max_particles))
	{
		_bottom_right = _top_left = remove_units(_center);
	}

	void Particle_emiter::update_center(Position center) {
		_center = center;
	}

	void Particle_emiter::update(bool active, Time dt, Environment_callback& env) {
		if(active) {
			spawn_new(dt);
		}

		auto di = simulate(dt, env);

		_particles.erase(di, _particles.end());
	}
	void Particle_emiter::spawn_new(Time dt) {
		_dt_acc+=dt;

		auto to_spawn = static_cast<std::size_t>(std::round(_spawn_rate * _dt_acc.value()));
		_dt_acc-=Time(to_spawn/_spawn_rate);

		to_spawn = glm::min(to_spawn, _max_particles-_particles.size());

		for(std::size_t i=0; i<to_spawn; ++i) {
			auto seed = next_pseed();

			_particles.emplace_back(
				remove_units(_center)+rand_point(_radius.value()),
				_direction(0, seed).value(),
				random_int(rng, _min_ttl, _max_ttl).value(), seed);
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
		auto t = (p.max_age-p.time_to_live) / p.max_age;

		p.color    = _color(t, p.seed);
		p.size     = remove_units(_size(t, p.seed));
		p.frame    = _frame(t, p.seed);

		p.velocity         += (_acceleration(t, p.seed)*dt).value();
		p.angular_velocity += (_angular_acceleration(t, p.seed)*dt).value();

		if(_physical) {
			if(env.handle(p.position, p.velocity, p.rotation))
				return false;
		}

		p.rotation += p.angular_velocity*dt;

		auto r = p.velocity * dt;
		p.position += glm::vec2{r*glm::cos(p.rotation), r*glm::sin(p.rotation)};

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
		     .set_uniform("layer", 0.8f);

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
		                           [](auto& pe){return pe.use_count()<=1;}),
		            _emiter.end());
	}

}
}
