#include "ConstantForceGenerator.hpp"

void Physics::ConstantForceGenerator::updateForce(PhysicsParticle* particle, float time) {
	srand(std::time(0));

	Vector accel = force * (rand() % 20 + 10);
	particle->addForce(accel);
}