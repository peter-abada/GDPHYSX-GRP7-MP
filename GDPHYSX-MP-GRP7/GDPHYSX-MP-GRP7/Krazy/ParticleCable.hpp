#ifndef PARTICLECABLE_HPP
#define PARTICLECABLE_HPP

#include "ParticleLink.hpp"
#include "PhysicsParticle.hpp"
#include "Vector.hpp"

namespace Krazy {
    class ParticleCable : public ParticleLink {
    public:
        float maxLength;
        float restitution;
        Vector anchorPoint;

        ParticleCable(PhysicsParticle* particle, Vector anchor, float maxLen, float rest)
            : anchorPoint(anchor), maxLength(maxLen), restitution(rest) {
            particles[0] = particle;
            particles[1] = nullptr; // Only one particle, other is anchor
        }

        ParticleContact* getContact() override;
    };
}

#endif
