#include "PhysicsWorld.hpp"
#include "ParticleCable.hpp" // Include for dynamic_cast
#include <cmath>

namespace Krazy {

    void PhysicsWorld::addContact(PhysicsParticle* p1, PhysicsParticle* p2, float restitution, Vector contactNormal, float depth) {
        ParticleContact* contact = new ParticleContact();
        contact->particles[0] = p1;
        contact->particles[1] = p2;
        contact->restitution = restitution;
        contact->contactNormal = contactNormal;
        contact->depth = depth;
        contacts.push_back(contact);
    }

    void PhysicsWorld::addParticle(PhysicsParticle* toAdd) {
        particles.push_back(toAdd);
        forceRegistry.add(toAdd, &gravity);
    }

    void PhysicsWorld::setGravity(const GravityForceGenerator& g) {
        gravity = g;
        // Re-register all particles with the new gravity generator
        forceRegistry.clear();
        for (auto* p : particles) {
            forceRegistry.add(p, &gravity);
        }
    }

    void PhysicsWorld::update(float time) {
        updateParticleList();

        // Apply all forces (gravity and others)
        forceRegistry.updateForces(time);

        // Integrate particle motion
        for (auto* p : particles) {
            p->update(time);
        }

        // Generate contacts (collisions, constraints)
        generateContacts();

        // Set enough iterations for stable resolution
        contactResolver.max_iterations = static_cast<unsigned>(particles.size() * 2);

        // Resolve contacts
        if (!contacts.empty()) {
            contactResolver.resolveContacts(contacts, time);
            // Clean up contacts after resolution
            for (auto* c : contacts) {
                delete c;
            }
            contacts.clear();
        }

        // Enforce hard cable constraints (guaranteed, no stretching)
        enforceCableConstraints();
    }

    void PhysicsWorld::enforceCableConstraints() {
        for (auto* link : links) {
            auto* cable = dynamic_cast<ParticleCable*>(link);
            if (cable && cable->particles[0]) {
                Vector dir = cable->particles[0]->position - cable->anchorPoint;
                float dist = dir.Magnitude();
                if (dist > cable->maxLength) {
                    Vector norm = dir.Normalize();
                    cable->particles[0]->position = cable->anchorPoint + norm * cable->maxLength;
                    // Remove velocity component away from anchor (prevents "pushing" out)
                    Vector v = cable->particles[0]->velocity;
                    float vDot = v.x * norm.x + v.y * norm.y + v.z * norm.z;
                    if (vDot > 0) {
                        cable->particles[0]->velocity = v - norm * vDot;
                    }
                }
            }
        }
    }

    void PhysicsWorld::updateParticleList() {
        for (auto it = particles.begin(); it != particles.end(); ) {
            if ((*it)->isDestroy()) {
                forceRegistry.remove(*it, &gravity);
                delete* it;
                it = particles.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void PhysicsWorld::generateContacts() {
        // Generate contacts from links (cables, rods, etc.)
        for (auto* link : links) {
            ParticleContact* contact = link->getContact();
            if (contact) {
                contacts.push_back(contact);
            }
        }
        // Handle particle-particle collisions
        getOverlaps();
    }

    void PhysicsWorld::getOverlaps()
    {
        int n = static_cast<int>(particles.size());
        for (int i = 0; i < n - 1; i++) {
            auto a = std::next(particles.begin(), i);
            if ((*a)->isDestroy()) continue;
            for (int h = i + 1; h < n; h++) {
                auto b = std::next(particles.begin(), h);
                if ((*b)->isDestroy()) continue;

                Vector mag2Vector = (*a)->position - (*b)->position;
                float mag2 = mag2Vector.squareMagnitude();
                float rad = (*a)->radius + (*b)->radius;
                float rad2 = rad * rad;

                if (mag2 <= rad2) {
                    Vector dir = mag2Vector.Normalize();
                    float r = rad2 - mag2;
                    float depth = sqrtf(r);
                    float restitution = fmin((*a)->restitution, (*b)->restitution);
                    addContact(*a, *b, restitution, dir, depth);
                }
            }
        }
    }

}
