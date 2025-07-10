#ifndef PHYSICSPARTICLE_HPP
#define PHYSICSPARTICLE_HPP

#include "Vector.hpp"

namespace Krazy {
	class PhysicsParticle {

	public:
		float mass = 0;

		Krazy::Vector position;

		Krazy::Vector velocity;

		Krazy::Vector accel;

		float lifespan;

		float damping = 0.9f;

		float radius = 1.0f;

		float restitution = 1.0f;

	protected:
		void updatePosition(float time);

		void updateVelocity(float time);

		bool isDestroyed = false;

		Krazy::Vector accumulatedForce = Krazy::Vector(0, 0, 0);

	public:
		void update(float time);

		void destroy();

		bool isDestroy();

		void addForce(Krazy::Vector force);
		void resetForce();
	};
}

#endif