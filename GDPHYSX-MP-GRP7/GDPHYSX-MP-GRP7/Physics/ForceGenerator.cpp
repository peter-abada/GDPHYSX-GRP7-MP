#include "ForceGenerator.hpp"

void Physics::ForceGenerator::updateForce(Physics::PhysicsParticle* p, float time) {
	p->addForce(Physics::Vector(0, 0, 0));
}