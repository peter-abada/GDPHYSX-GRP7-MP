#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <list>
#include "PhysicsParticle.hpp"
#include "ForceRegistry.hpp"
#include "GravityForceGenerator.hpp"

namespace Physics {
	class PhysicsWorld {
	public:
		ForceRegistry forceRegistry;

		std::list<PhysicsParticle*> particles;

		void addParticle(PhysicsParticle* toAdd);

		void update(float time);


	private:
		void updateParticleList();

		GravityForceGenerator gravity = GravityForceGenerator(Vector(0, -9.8f, 0));
	};
}

#endif