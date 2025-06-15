#include "ForceRegistry.hpp"

void Krazy::ForceRegistry::add(PhysicsParticle* particle, ForceGenerator* generator) {
	particleForceRegistry toAdd;
	toAdd.particle = particle;
	toAdd.generator = generator;

	registry.push_back(toAdd);
}

void Krazy::ForceRegistry::clear() {
	registry.clear();
}

void Krazy::ForceRegistry::remove(PhysicsParticle* particle, ForceGenerator* generator) {
	registry.remove_if(
		[particle, generator](particleForceRegistry reg) {
			return reg.particle == particle && reg.generator == generator;
		}
	);
}

void Krazy::ForceRegistry::updateForces(float time) {
	for (std::list<particleForceRegistry>::iterator i = registry.begin();
		i != registry.end();
		i++) {
		i->generator->updateForce(i->particle, time);
	}
}