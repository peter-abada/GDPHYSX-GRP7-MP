#include "PhysicsWorld.hpp"
namespace Krazy {
	void PhysicsWorld::addParticle(PhysicsParticle* toAdd) {
		particles.push_back(toAdd);

		forceRegistry.add(toAdd, &gravity);
	}

	void PhysicsWorld::update(float time) {
		updateParticleList();

		forceRegistry.updateForces(time);

		for (std::list<PhysicsParticle*>::iterator p = particles.begin();
			p != particles.end();
			p++) {
			(*p)->update(time);
		}

		generateContacts();

		if (contacts.size() > 0) {
			contactResolver.resolveContacts(contacts, time);
		}
	}

	void PhysicsWorld::updateParticleList() {
		particles.remove_if([](PhysicsParticle* p) {
			return p->isDestroy();
			});
	}

	void PhysicsWorld::addContact(PhysicsParticle* p1, PhysicsParticle* p2, float restitution, Vector contactNormal, float depth) {
		ParticleContact* toAdd = new ParticleContact();

		toAdd->particles[0] = p1;
		toAdd->particles[1] = p2;
		toAdd->restitution = restitution;
		toAdd->contactNormal = contactNormal;
		toAdd->depth = depth;

		contacts.push_back(toAdd);
	}

	void PhysicsWorld::generateContacts() {
		contacts.clear();

		getOverlaps();

		for (std::list<ParticleLink*>::iterator i = links.begin(); i != links.end(); i++) {
			ParticleContact* contact = (*i)->getContact();

			if (contact != nullptr)
				contacts.push_back(contact);
		}
	}

	void PhysicsWorld::getOverlaps() {
		for (int i = 0; i < particles.size() - 1; i++) {
			std::list<PhysicsParticle*>::iterator a = std::next(particles.begin(), i);

			for (int h = i + 1; h < particles.size(); h++) {
				std::list<PhysicsParticle*>::iterator b = std::next(particles.begin(), h);

				Vector mag2Vector = (*a)->position - (*b)->position;
				float mag2 = mag2Vector.squareMagnitude();

				float rad = (*a)->radius + (*b)->radius;
				float rad2 = rad * rad;

				if (mag2 <= rad2) {
					Vector dir = mag2Vector.Normalize();

					float r = rad2 - mag2;
					float depth = sqrt(r);

					float restitution = fmin((*a)->restitution, (*b)->restitution);

					addContact(*a, *b, restitution, dir, depth);
				}
			}
		}

	}
}
