#include "ParticleContact.hpp"
#include "PhysicsParticle.hpp"

namespace Krazy {

    void ParticleContact::resolve(float time) {
        resolveVelocity(time);
        resolveInterpenetration(time);
    }

    float ParticleContact::getSeparatingSpeed() const {
        Vector relativeVel = particles[0]->velocity;
        if (particles[1]) relativeVel = relativeVel - particles[1]->velocity;
        return relativeVel.x * contactNormal.x + relativeVel.y * contactNormal.y + relativeVel.z * contactNormal.z;
    }

    void ParticleContact::resolveVelocity(float time) {
        float separatingSpeed = getSeparatingSpeed();
        if (separatingSpeed > 0) return;

        float newSepSpeed = -separatingSpeed * restitution;

        float totalInverseMass = 0.0f;
        if (particles[0] && particles[0]->mass > 0.0f)
            totalInverseMass += 1.0f / particles[0]->mass;
        if (particles[1] && particles[1]->mass > 0.0f)
            totalInverseMass += 1.0f / particles[1]->mass;

        if (totalInverseMass <= 0.0f) return;

        float deltaSpeed = newSepSpeed - separatingSpeed;
        float impulse = deltaSpeed / totalInverseMass;
        Vector impulsePerIMass = contactNormal * impulse;

        if (particles[0] && particles[0]->mass > 0.0f)
            particles[0]->velocity = particles[0]->velocity + impulsePerIMass * (1.0f / particles[0]->mass);
        if (particles[1] && particles[1]->mass > 0.0f)
            particles[1]->velocity = particles[1]->velocity - impulsePerIMass * (1.0f / particles[1]->mass);
    }

    void ParticleContact::resolveInterpenetration(float time) {
        if (depth <= 0.0f) return;

        float totalInverseMass = 0.0f;
        if (particles[0] && particles[0]->mass > 0.0f)
            totalInverseMass += 1.0f / particles[0]->mass;
        if (particles[1] && particles[1]->mass > 0.0f)
            totalInverseMass += 1.0f / particles[1]->mass;

        if (totalInverseMass <= 0.0f) return;

        Vector movePerIMass = contactNormal * (depth / totalInverseMass);

        if (particles[0] && particles[0]->mass > 0.0f)
            particles[0]->position = particles[0]->position + movePerIMass * (1.0f / particles[0]->mass);
        if (particles[1] && particles[1]->mass > 0.0f)
            particles[1]->position = particles[1]->position - movePerIMass * (1.0f / particles[1]->mass);
    }

}
