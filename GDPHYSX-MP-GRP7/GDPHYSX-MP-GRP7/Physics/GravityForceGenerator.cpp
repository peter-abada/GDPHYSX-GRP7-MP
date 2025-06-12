#include "GravityForceGenerator.hpp"

void Physics::GravityForceGenerator::updateForce(PhysicsParticle* particle, float time) {
	if (particle->mass <= 0) return;

	Vector force = Gravity * particle->mass;
	particle->addForce(force);
}