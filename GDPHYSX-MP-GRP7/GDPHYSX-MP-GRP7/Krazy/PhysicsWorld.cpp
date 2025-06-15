#include "PhysicsWorld.hpp"

void Krazy::PhysicsWorld::addParticle(Krazy::PhysicsParticle* toAdd) {
	particles.push_back(toAdd);

	forceRegistry.add(toAdd, &gravity);
}

void Krazy::PhysicsWorld::update(float time) {
	updateParticleList();

	forceRegistry.updateForces(time);

	for (std::list<Krazy::PhysicsParticle*>::iterator p = particles.begin();
		p != particles.end();
		p++) {
		(*p)->update(time);
	}
}

void Krazy::PhysicsWorld::updateParticleList() {
	particles.remove_if([](PhysicsParticle* p) {
		return p->isDestroy();
		});
}