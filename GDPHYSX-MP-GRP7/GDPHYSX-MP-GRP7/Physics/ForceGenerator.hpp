#ifndef FORCEGENERATOR_HPP
#define FORCEGENERATOR_HPP

#include "PhysicsParticle.hpp"

namespace Physics {
	class ForceGenerator {
	public:
		virtual void updateForce(Physics::PhysicsParticle* p, float time);
	};
}

#endif