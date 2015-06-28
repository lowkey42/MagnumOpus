#include "particle_emiter_comp.hpp"

#include <sf2/sf2.hpp>
#include <core/ecs/serializer_impl.hpp>

#include "../../sys/physics/transform_comp.hpp"

namespace mo {
namespace sys {
namespace graphic {

	using namespace util;
	using namespace unit_literals;


	struct Particle_emiter_comp::Persisted_state {
		Particle_emiter_type type;
		bool enabled=true;

		Persisted_state(const Particle_emiter_comp& p) : type(p._type), enabled(p._enabled) {}
	};

	sf2_enumDef(Particle_emiter_type,
		sf2_value(element_fire),
		sf2_value(element_fire),
		sf2_value(element_frost),
		sf2_value(element_water),
		sf2_value(element_stone),
		sf2_value(element_gas),
		sf2_value(element_lightning),
		sf2_value(health)
	)

	sf2_structDef(Particle_emiter_comp::Persisted_state,
		sf2_member(type),
		sf2_member(enabled)
	)

	void Particle_emiter_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});
		particle_type(s.type) ;
		enabled(s.enabled);
	}
	void Particle_emiter_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this});
	}

	void Particle_emiter_comp::enabled(bool e) {
		_enabled = e;
		if(_emiter)
			_emiter->active(e);
	}
	void Particle_emiter_comp::particle_type(Particle_emiter_type t) {
		if(_type!=t) {
			_type = t;
			_emiter.reset();
		}
	}

	void Particle_emiter_comp::scale(Distance r) {
		if(_emiter) {
			r/=1.5f;
			auto rate = r /0.25_m;

			_emiter->radius(r);
			_emiter->spawn(200 * rate, 400 * rate);
			_emiter->ttl(0.3_s * rate, 0.6_s * rate);
			_emiter->acceleration(
				util::lerp<Speed_per_time>(8_m/second_2, 0_m/second_2) /rate,
				util::lerp<Angle_acceleration>(3000_deg/second_2, 0_deg/second_2) /rate
			);
		}
	}

	void Particle_emiter_comp::_create_emiter(renderer::Particle_renderer& pr,
	                                          asset::Asset_manager& assets) {
		if(!_emiter && _enabled) {
			renderer::Texture_ptr tex;
			switch(_type) {
				case Particle_emiter_type::element_fire:
					tex = assets.load<renderer::Texture>("tex:particle_fire"_aid);
					break;
				case Particle_emiter_type::element_frost:
					tex = assets.load<renderer::Texture>("tex:particle_frost"_aid);
					break;
				case Particle_emiter_type::element_water:
					tex = assets.load<renderer::Texture>("tex:particle_water"_aid);
					break;
				case Particle_emiter_type::element_stone:
					tex = assets.load<renderer::Texture>("tex:particle_stone"_aid);
					break;
				case Particle_emiter_type::element_gas:
					tex = assets.load<renderer::Texture>("tex:particle_gas"_aid);
					break;
				case Particle_emiter_type::element_lightning:
					tex = assets.load<renderer::Texture>("tex:particle_lightning"_aid);
					break;
				case Particle_emiter_type::health:
					tex = assets.load<renderer::Texture>("tex:particle_health"_aid);
					break;
			}

			if(tex) {
				_emiter = pr.create_emiter(
						owner().get<physics::Transform_comp>().get_or_throw().position(),
						0_deg,
						0.5_m,
						renderer::Collision_handler::none,
						100,
						200,
						0.5_s, 0.8_s,
						util::scerp<Angle>(0_deg, 0_deg),
						util::scerp<Angle>(0_deg, 0_deg),
						util::lerp<Speed_per_time>(8_m/second_2, 0_m/second_2),
						util::lerp<Angle_acceleration>(3000_deg/second_2, 0_deg/second_2),
						util::lerp<glm::vec4>({1,0.5,0.5,0}, {0,0,0,0.0}),
						util::lerp<Position>({25_cm, 25_cm}, {60_cm, 60_cm}, {2_cm, 2_cm}),
						util::scerp<int8_t>(0),
						tex
				);
			}

		}
	}
}
}
}
