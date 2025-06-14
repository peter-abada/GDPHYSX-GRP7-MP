#include "ConstantForceGenerator.hpp"

void Physics::ConstantForceGenerator::updateForce(PhysicsParticle* particle, float time) {
	srand(std::time(0));

	Vector accel = force * 5;
	particle->addForce(accel);
}