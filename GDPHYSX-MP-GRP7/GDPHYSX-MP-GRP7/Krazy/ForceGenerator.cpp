#include "ForceGenerator.hpp"

void Krazy::ForceGenerator::updateForce(Krazy::PhysicsParticle* p, float time) {
	p->addForce(Krazy::Vector(0, 0, 0));
}