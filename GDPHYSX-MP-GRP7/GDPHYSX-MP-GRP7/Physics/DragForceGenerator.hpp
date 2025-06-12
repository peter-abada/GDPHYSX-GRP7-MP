#ifndef DRAGFORCEGENERATOR_HPP
#define DRAGFORCEGENERATOR_HPP

#include "ForceGenerator.hpp"

namespace Physics {
	class DragForceGenerator : public ForceGenerator {
	private:
		float k1 = 0.74f;	
		float k2 = 0.57f;

	public:
		DragForceGenerator() {}
		DragForceGenerator(float _k1, float _k2) : k1(_k1), k2(_k2){}

		void updateForce(PhysicsParticle* particle, float time) override;
	};
}

#endif