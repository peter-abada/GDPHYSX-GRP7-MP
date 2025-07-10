#include "Rod.hpp"

namespace Krazy {
	ParticleContact* Rod::getContact() {
		float currLen = currentLength();

		if (currLen == length) {
			return nullptr;
		}

		ParticleContact* ret = new ParticleContact();
		ret->particles[0] = particles[0];
		ret->particles[1] = particles[1];

		Vector dir = particles[1]->position - particles[0]->position;
		dir = dir.Normalize();

		if (currLen > length) {
			ret->contactNormal = dir;
			ret->depth = currLen - length;
		}
		else {
			ret->contactNormal = dir * -1;
			ret->depth = length - currLen;
		}

		ret->restitution = restitution;

		return ret;
	}
}