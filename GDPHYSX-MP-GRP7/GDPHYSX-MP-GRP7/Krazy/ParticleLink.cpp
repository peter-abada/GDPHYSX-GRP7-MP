#include "ParticleLink.hpp"

namespace Krazy {
	float ParticleLink::currentLength() {
		Vector ret = particles[0]->position - particles[1]->position;
		return ret.Magnitude();
	}
}