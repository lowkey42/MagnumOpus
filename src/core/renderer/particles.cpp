#include "particles.hpp"


namespace mo {
namespace renderer {

	namespace {
		Vertex_layout particle_vertex_layout {
			Vertex_layout::Mode::triangles,
			vertex("position", &Particle::position),
			vertex("color", &Particle::color),
			vertex("size", &Particle::size),
			vertex("rotation", &Particle::rotation),
			vertex("ttl", &Particle::time_to_live),
			vertex("max_age", &Particle::max_age)
		};

		struct Default_env_callback : Environment_callback {
			void handle(Position&, Velocity&)const noexcept {}
		};
	}

	Particle::Particle(glm::vec2 pos, glm::vec4 color, glm::vec2 size, float rotation,
	                   glm::vec2 vel, float rot_vel, float ttl)
	    : position(pos), color(color), size(size), rotation(rotation),
	      velocity(vel), angular_velocity(rot_vel), time_to_live(ttl), max_age(ttl)
	{
	}

	Particle_emiter::Particle_emiter(
	        Position center, Distance radius,
	        bool physical, bool aligned,
	        float spawn_rate, std::size_t max_particles,
	        Time min_ttl, Time max_ttl,
	        util::Xerp<Angle> direction,
	        util::Xerp<Speed_per_time> acceleration,
	        util::Xerp<Angle_acceleration> angular_acceleration,
	        util::Xerp<glm::vec4> color,
	        util::Xerp<Position> size)
	    : _center(center), _radius(radius), _spawn_rate(spawn_rate),
	      _physical(physical), _aligned(aligned),
	      _min_ttl(min_ttl), _max_ttl(max_ttl), _direction(direction),
	      _acceleration(acceleration), _angular_acceleration(angular_acceleration),
	      _color(color), _size(size),
	      _obj(particle_vertex_layout, create_dynamic_buffer<Particle>(max_particles))
	{
	}

	void Particle_emiter::update_center(Position center) {
		_center = center;
	}

	void Particle_emiter::update(bool active, Time dt, Environment_callback& env) {
		// TODO
	}

	void Particle_emiter::draw() {
		// TODO
	}
	bool Particle_emiter::visible(glm::vec2 top_left, glm::vec2 bottom_right)const noexcept {
		// TODO
		return false;
	}
	bool Particle_emiter::empty()const noexcept {
		// TODO
		return false;
	}


	Particle_renderer::Particle_renderer(asset::Asset_manager& assets,
	                                     std::unique_ptr<Environment_callback> env)
	    : _env(env ? std::move(env) : std::make_unique<Default_env_callback>())
	{

	}

	void Particle_renderer::draw(Time dt, glm::vec2 top_left, glm::vec2 bottom_right) {
		for(auto& pe : _emiter) {
			if(pe->visible(top_left, bottom_right)) {
				pe->update(pe.use_count()==1, dt, *_env);
				pe->draw();
			}
		}

		_emiter.erase(
		            std::remove_if(_emiter.begin(),
		                           _emiter.end(),
		                           [](auto& pe){return pe.use_count()<=1;}),
		            _emiter.end());
	}

}
}
