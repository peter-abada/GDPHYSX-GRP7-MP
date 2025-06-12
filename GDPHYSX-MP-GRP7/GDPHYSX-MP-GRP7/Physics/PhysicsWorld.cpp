#include "PhysicsWorld.hpp"

void Physics::PhysicsWorld::addParticle(Physics::PhysicsParticle* toAdd) {
	particles.push_back(toAdd);

	//forceRegistry.add(toAdd, &gravity);
}

void Physics::PhysicsWorld::update(float time) {
	updateParticleList();

	forceRegistry.updateForces(time);

	for (std::list<Physics::PhysicsParticle*>::iterator p = particles.begin();
		p != particles.end();
		p++) {
		(*p)->update(time);
	}
}

void Physics::PhysicsWorld::updateParticleList() {
	particles.remove_if([](PhysicsParticle* p) {
		return p->isDestroy();
		});
}