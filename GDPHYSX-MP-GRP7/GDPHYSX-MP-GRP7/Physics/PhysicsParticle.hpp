#ifndef PHYSICSPARTICLE_HPP
#define PHYSICSPARTICLE_HPP

#include "Vector.hpp"

namespace Physics {
	class PhysicsParticle {

	public:
		float mass = 0;

		Physics::Vector position;

		Physics::Vector velocity;

		Physics::Vector accel;

		float lifespan;

		float damping = 0.9f;

	protected:
		void updatePosition(float time);

		void updateVelocity(float time);

		bool isDestroyed = false;

		Physics::Vector accumulatedForce = Physics::Vector(0, 0, 0);

	public:
		void update(float time);

		void destroy();

		bool isDestroy();

		void addForce(Physics::Vector force);
		void resetForce();
	};
}

#endif