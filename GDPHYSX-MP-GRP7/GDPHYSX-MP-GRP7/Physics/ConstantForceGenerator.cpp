#include "ConstantForceGenerator.hpp"

void Physics::ConstantForceGenerator::updateForce(PhysicsParticle* particle, float time) {
	srand(std::time(0));

	Vector accel = force * 3;
	particle->addForce(accel);
}