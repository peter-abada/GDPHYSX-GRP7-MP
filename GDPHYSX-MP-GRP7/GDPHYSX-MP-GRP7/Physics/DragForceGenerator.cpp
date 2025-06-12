#include "DragForceGenerator.hpp"

void Physics::DragForceGenerator::updateForce(PhysicsParticle* particle, float time) {
	Vector force = Vector(0, 0, 0);
	Vector currentV = particle->velocity;

	float mag = currentV.Magnitude();
	if (mag <= 0) return;

	float dragForce = (k1 * mag) + (k2 * mag);
	Vector dir = currentV.Normalize();

	particle->addForce(dir * -dragForce);
}