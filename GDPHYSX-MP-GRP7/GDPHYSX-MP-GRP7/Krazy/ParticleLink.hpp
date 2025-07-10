#ifndef PARTICLELINK_HPP
#define PARTICLELINK_HPP

#include "PhysicsParticle.hpp"
#include "ParticleContact.hpp"

namespace Krazy {
	class ParticleLink {
	public:
		PhysicsParticle* particles[2];

		virtual ParticleContact* getContact() { return nullptr; }

	protected:
		float currentLength();
	};
}

#endif