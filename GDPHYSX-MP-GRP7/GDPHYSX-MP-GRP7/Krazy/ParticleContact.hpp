#ifndef PARTICLECONTACT_HPP
#define PARTICLECONTACT_HPP

#include "PhysicsParticle.hpp"

namespace Krazy {
	class ParticleContact {
	public:
		PhysicsParticle* particles[2];
		
		float restitution;

		Vector contactNormal;

		void resolve(float time);

		float depth;

	//protected:
		float getSeparatingSpeed();

		void resolveVelocity(float time);

		void resolveInterpenetration(float time);
	};
}

#endif