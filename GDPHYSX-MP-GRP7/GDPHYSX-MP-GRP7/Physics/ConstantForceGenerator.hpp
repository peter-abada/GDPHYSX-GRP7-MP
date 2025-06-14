#ifndef CONSTANTFORCEGENERATOR_HPP
#define CONSTANTFORCEGENERATOR_HPP

#include "ForceGenerator.hpp"
#include <cstdlib>
#include <ctime>

namespace Physics {
	class ConstantForceGenerator : public ForceGenerator {
	private:
		Vector force = Vector(0, 1, 0);

	public:
		ConstantForceGenerator(Vector _force) : force(_force) {}

		void updateForce(PhysicsParticle* particle, float time) override;
	};
}

#endif