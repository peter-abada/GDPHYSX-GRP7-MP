#include "ParticleCable.hpp"
#include <cmath>

using namespace Krazy;

ParticleContact* ParticleCable::getContact() {
    Vector pos = particles[0]->position;
    Vector diff = pos - anchorPoint;
    float len = diff.Magnitude();

    if (len < maxLength) return nullptr;

    ParticleContact* contact = new ParticleContact();
    contact->particles[0] = particles[0];
    contact->particles[1] = nullptr;
    contact->contactNormal = diff.Normalize();
    contact->depth = len - maxLength;
    contact->restitution = restitution;
    return contact;
}
