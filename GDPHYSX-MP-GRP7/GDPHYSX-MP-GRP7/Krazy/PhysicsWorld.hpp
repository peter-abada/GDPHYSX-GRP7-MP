#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <list>
#include <vector>
#include "PhysicsParticle.hpp"
#include "ForceRegistry.hpp"
#include "GravityForceGenerator.hpp"
#include "ParticleContact.hpp"
#include "ContactResolver.hpp"
#include "ParticleLink.hpp"

namespace Krazy {
	class PhysicsWorld {
	public:
		std::vector<ParticleContact*> contacts;

		void addContact(PhysicsParticle* p1, PhysicsParticle* p2, float restitution, Vector contactNormal, float depth);

		std::list<ParticleLink*> links;

		ForceRegistry forceRegistry;

		std::list<PhysicsParticle*> particles;

		void addParticle(PhysicsParticle* toAdd);

		void update(float time);

	protected:
		ContactResolver contactResolver = ContactResolver(20);

		void generateContacts();

		void getOverlaps();

	private:
		void updateParticleList();

		GravityForceGenerator gravity = GravityForceGenerator(Vector(0, -9.8f, 0));
	};
}

#endif