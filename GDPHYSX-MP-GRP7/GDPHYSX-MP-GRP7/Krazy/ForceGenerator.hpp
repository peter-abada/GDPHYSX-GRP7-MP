#ifndef FORCEGENERATOR_HPP
#define FORCEGENERATOR_HPP

#include "PhysicsParticle.hpp"

namespace Krazy {
	class ForceGenerator {
	public:
		virtual void updateForce(Krazy::PhysicsParticle* p, float time);
	};
}

#endif