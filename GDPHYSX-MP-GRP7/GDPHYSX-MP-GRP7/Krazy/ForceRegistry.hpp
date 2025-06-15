#ifndef FORCEREGISTRY_HPP
#define FORCEREGISTRY_HPP

#include "PhysicsParticle.hpp"
#include "ForceGenerator.hpp"
#include "list"

namespace Krazy {
	class ForceRegistry{
	public:
		void add(PhysicsParticle* particle, ForceGenerator* generator);
		void remove(PhysicsParticle* particle, ForceGenerator* generator);
		void clear();
		void updateForces(float time);

	protected:
		struct particleForceRegistry {
			PhysicsParticle* particle;
			ForceGenerator* generator;
		};

		std::list<particleForceRegistry> registry;
	};
}

#endif