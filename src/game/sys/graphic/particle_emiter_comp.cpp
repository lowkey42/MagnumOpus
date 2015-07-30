#define MO_BUILD_SERIALIZER

#include "particle_emiter_comp.hpp"

#include <sf2/sf2.hpp>
#include <core/ecs/serializer_impl.hpp>

#include "../../sys/physics/transform_comp.hpp"

namespace mo {
namespace sys {
namespace graphic {

	using namespace util;
	using namespace unit_literals;


	struct Emiter_state {
		Effect_type type=Effect_type::none;
		bool enabled=true;
		bool scale=false;
		bool temporary=false;
	};

	struct Particle_emiter_comp::Persisted_state {
		std::vector<Emiter_state> emiters;

		Persisted_state(const Particle_emiter_comp& p, bool save=false) {
/* Particle Emiters are currently not serialised */

			if(save) {
				emiters.reserve(max_emiters);
				for(auto i : range(max_emiters)) {
					auto& e = p._emiters[i];
					emiters.push_back({e._type, e._enabled, e._scale});
				}
			}
		}
	};

	sf2_structDef(Emiter_state,
		sf2_member(type),
		sf2_member(enabled),
		sf2_member(scale)
	)
	sf2_structDef(Particle_emiter_comp::Persisted_state,
		sf2_member(emiters)
	)

	void Particle_emiter_comp::load(ecs::Entity_state& state) {
		auto s = state.read_to(Persisted_state{*this});

		for(auto i : range(max_emiters)) {
			if(i>=s.emiters.size())
				continue;

			auto& se = s.emiters.at(i);

			particle_type(i, se.type, se.scale);
			enabled(i, se.enabled);
		}
	}
	void Particle_emiter_comp::store(ecs::Entity_state& state) {
		state.write_from(Persisted_state{*this, true});
	}

	void Particle_emiter_comp::enabled(std::size_t i, bool e, bool temp) {
		INVARIANT(i<max_emiters, "accessd particle emiter out of range");

		_emiters[i]._enabled = e;
		_emiters[i]._temporary = temp;
		_emiters[i]._to_be_disabled = false;
		if(_emiters[i]._emiter)
			_emiters[i]._emiter->active(e);
	}
	auto Particle_emiter_comp::enabled(std::size_t i)const noexcept {
		INVARIANT(i<max_emiters, "accessd particle emiter out of range");

		return _emiters[i]._enabled;
	}
	void Particle_emiter_comp::particle_type(std::size_t i,
	                                         Effect_type t,
	                                         bool scale) {
		INVARIANT(i<max_emiters, "accessd particle emiter out of range");

		if(_emiters[i]._type!=t) {
			_emiters[i]._type = t;
			_emiters[i]._emiter.reset();
			_emiters[i]._scale = scale;
		}
	}
	auto Particle_emiter_comp::particle_type(std::size_t i)const noexcept {
		INVARIANT(i<max_emiters, "accessd particle emiter out of range");

		return _emiters[i]._type;
	}

	void Particle_emiter_comp::scale(std::size_t i, Distance r) {
		INVARIANT(i<max_emiters, "accessd particle emiter out of range");

		if(_emiters[i]._emiter) {
			r/=1.5f;
			auto rate = r /0.25_m;

			_emiters[i]._emiter->radius(r);
			_emiters[i]._emiter->spawn(200 * rate, 400 * rate);
			_emiters[i]._emiter->ttl(0.3_s * rate, 0.6_s * rate);
			_emiters[i]._emiter->acceleration(
				util::lerp<Speed_per_time>(8_m/second_2, 0_m/second_2) /rate,
				util::lerp<Angle_acceleration>(3000_deg/second_2, 0_deg/second_2) /rate
			);
		}
	}
}
}
}
