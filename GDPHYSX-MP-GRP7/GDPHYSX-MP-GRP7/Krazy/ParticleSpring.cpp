#include "ParticleSpring.hpp"

namespace Krazy {
	void ParticleSpring::updateForce(PhysicsParticle* particle, float time) {
		Vector pos = particle->position;

		Vector force = pos - otherParticle->position;

		float mag = force.Magnitude();

		float springForce = -springConstant * abs(mag - restLength);

		force = force.Normalize();
		force = force * springForce;

		particle->addForce(force);
	}
}