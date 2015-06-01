#include "particles.hpp"

#include "primitives.hpp"
#include "graphics_ctx.hpp"

namespace mo {
namespace renderer {

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
			void handle(Position&, Velocity&)const noexcept {}
		};

	}

	Particle::Particle(glm::vec2 pos, glm::vec4 color, glm::vec2 size, float rotation,
	                   glm::vec2 vel, float rot_vel, float frame, float ttl)
	    : position(pos), color(color), size(size), rotation(rotation), frame(frame),
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
	        util::Xerp<Position> size,
	        util::Xerp<int8_t> frame,
	        Texture_ptr texture)
	    : _center(center), _radius(radius), _spawn_rate(spawn_rate),
	      _physical(physical), _aligned(aligned),
	      _min_ttl(min_ttl), _max_ttl(max_ttl), _frame(frame), _direction(direction),
	      _acceleration(acceleration), _angular_acceleration(angular_acceleration),
	      _color(color), _size(size), _texture(texture),
	      _obj(particle_vertex_layout, create_buffer(particle_vertices),
	           create_dynamic_buffer<Particle>(max_particles))
	{
		for(int i=0; i<10000; i++)
		_particles.emplace_back(remove_units(center)+glm::vec2{i/100.f, i/100.f}, glm::vec4(1,0,0,0.1), glm::vec2{1,1}, 0, glm::vec2{0,0}, 0, 0, 1);
	}

	void Particle_emiter::update_center(Position center) {
		_center = center;
	}

	void Particle_emiter::update(bool active, Time dt, Environment_callback& env) {
		// TODO
		if(_physical && !_aligned)
			_spawn_rate++;

	}

	void Particle_emiter::draw(Shader_program& prog) {
		if(_texture)
			_texture->bind();

		prog.set_uniform("frames", static_cast<float>(_frame.max()));

		_obj.buffer(1).set(_particles);
		_obj.draw();
	}
	bool Particle_emiter::visible(glm::vec2 top_left, glm::vec2 bottom_right)const noexcept {
		return true;/*
		return !(
		            top_left.x     > _bottom_right.x ||
		            bottom_right.x < _top_left.x ||
		            top_left.y     > _bottom_right.y ||
		            bottom_right.y < _top_left.y
		        );*/
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
				pe->update(pe.use_count()==1, dt, *_env);
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
