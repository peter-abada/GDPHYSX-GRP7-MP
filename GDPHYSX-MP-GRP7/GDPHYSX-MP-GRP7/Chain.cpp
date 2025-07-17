// This is a chain because it only prevents the particle from moving farther than its max length from the anchor.
// No force is applied if the particle is within the chain's length.

#include "Chain.hpp"
#include "Krazy/ParticleContact.hpp"

namespace Krazy {
    ParticleContact* Chain::getContact() {
        if (!particles[0]) return nullptr;
        Vector delta = particles[0]->position - anchor;
        float length = delta.Magnitude();
        if (length <= maxLength) return nullptr;

        ParticleContact* contact = new ParticleContact();
        contact->particles[0] = particles[0];
        contact->particles[1] = nullptr;
        contact->contactNormal = delta.Normalize();
        contact->restitution = 0.0f;
        contact->depth = length - maxLength;
        return contact;
    }
}
