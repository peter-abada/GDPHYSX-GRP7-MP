#ifndef ANCHOREDSPRING_HPP
#define ANCHOREDSPRING_HPP

#include "ForceGenerator.hpp"
#include "PhysicsParticle.hpp"

namespace Krazy {
	class AnchoredSpring : public ForceGenerator {
	private:
		Vector anchorPoint;

		float springConstant;

		float restLength;

	public:
		AnchoredSpring(Vector pos, float _springConst, float _restLen) :
			anchorPoint(pos), springConstant(_springConst), restLength(_restLen) {}

		void updateForce(PhysicsParticle* particle, float time) override;
	};
}

#endif