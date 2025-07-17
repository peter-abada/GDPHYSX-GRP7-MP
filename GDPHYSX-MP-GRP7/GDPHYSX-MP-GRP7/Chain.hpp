#pragma once
#include "Krazy/Vector.hpp"
#include "Krazy/PhysicsParticle.hpp"
#include "Krazy/ParticleContact.hpp"
#include "Krazy/ParticleLink.hpp"

namespace Krazy {
    class Chain : public ParticleLink {
    public:
        Vector anchor;      // The fixed anchor point for the chain
        float maxLength;    // The maximum allowed length of the chain
        float restitution = 0.0f;

        Chain() : anchor(0, 0, 0), maxLength(1.0f) {
            particles[0] = nullptr;
            particles[1] = nullptr;
        }

        Chain(PhysicsParticle* particle, Vector anchor, float maxLen, float rest = 0.0f)
            : anchor(anchor), maxLength(maxLen), restitution(rest) {
            particles[0] = particle;
            particles[1] = nullptr;
        }

        ParticleContact* getContact() override;
    };
}
