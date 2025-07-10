#ifndef PARTICLESPRING_HPP
#define PARTICLESPRING_HPP

#include "ForceGenerator.hpp"
#include "PhysicsParticle.hpp"

namespace Krazy {
	class ParticleSpring : public ForceGenerator {
	private:
		PhysicsParticle* otherParticle;

		float springConstant;

		float restLength;

	public:
		ParticleSpring(PhysicsParticle* particle, float _springConst, float _restLen) :
			otherParticle(particle), springConstant(_springConst), restLength(_restLen) {}

		void updateForce(PhysicsParticle* particle, float time) override;
	};
}

#endif