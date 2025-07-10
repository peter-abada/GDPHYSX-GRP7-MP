#include "AnchoredSpring.hpp"

namespace Krazy {
	void AnchoredSpring::updateForce(PhysicsParticle* particle, float time) {
		Vector pos = particle->position;

		Vector force = pos - anchorPoint;

		float mag = force.Magnitude();

		float springForce = -springConstant * abs(mag - restLength);

		force = force.Normalize();

		force = force * springForce;

		particle->addForce(force);
	}
}