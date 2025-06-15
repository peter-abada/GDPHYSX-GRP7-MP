#ifndef GRAVITYFORCEGENERATOR_HPP
#define GRAVITYFORCEGENERATOR_HPP

#include "ForceGenerator.hpp"

namespace Krazy {
	class GravityForceGenerator : public ForceGenerator {
	private:
		Vector Gravity = Vector(0, -9.8, 0);

	public:
		GravityForceGenerator(const Vector gravity) : Gravity(gravity){}

		void updateForce(PhysicsParticle* particle, float time) override;
	};
}

#endif