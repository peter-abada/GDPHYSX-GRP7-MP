#include "ContactResolver.hpp"

void Krazy::ContactResolver::resolveContacts(std::vector<ParticleContact*> contacts, float time) {

	while (current_iterations != max_iterations) {
		unsigned int contactIndex = 0;
		float minSS = 0;
		float maxDepth = 0;
		for (unsigned int i = 0; i < contacts.size(); i++) {
			if (contacts[i]->getSeparatingSpeed() < minSS) {
				minSS = contacts[i]->getSeparatingSpeed();
				contactIndex = i;
			}
			if (contacts[i]->depth < maxDepth) {
				maxDepth = contacts[i]->depth;
				contactIndex = i;
			}
		}

		contacts[contactIndex]->resolve(time);
		if (minSS >= 0 && maxDepth <= 0) break;

		current_iterations++;
	}
}