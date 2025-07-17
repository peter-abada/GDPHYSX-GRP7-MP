#ifndef PARTICLECONTACT_HPP
#define PARTICLECONTACT_HPP

#include "Vector.hpp"

namespace Krazy {

    class PhysicsParticle;

    class ParticleContact {
    public:
        PhysicsParticle* particles[2] = { nullptr, nullptr };
        float restitution = 1.0f;
        Vector contactNormal;
        float depth = 0.0f;

        void resolve(float time);
        float getSeparatingSpeed() const;
        void resolveVelocity(float time);
        void resolveInterpenetration(float time);
    };

}

#endif
