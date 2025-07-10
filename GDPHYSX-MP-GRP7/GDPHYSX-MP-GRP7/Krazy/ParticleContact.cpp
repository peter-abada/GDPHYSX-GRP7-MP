#include "ParticleContact.hpp"

float Krazy::ParticleContact::getSeparatingSpeed() {
	Vector velocity = particles[0]->velocity;

	if (particles[1]) 
		velocity -= particles[1]->velocity;

	return velocity , contactNormal;
}

void Krazy::ParticleContact::resolveVelocity(float time) {
	float separatingSpeed = getSeparatingSpeed();

	if (separatingSpeed > 0) return;

	float newSS = -restitution * separatingSpeed;

	float deltaSpeed = newSS - separatingSpeed;

	float totalMass = (float)1 / particles[0]->mass;
	if (particles[1])
		totalMass += (float)1 / particles[1]->mass;

	if (totalMass <= 0) return;

	float impulse_mag = deltaSpeed / totalMass;
	Vector impulse = contactNormal * impulse_mag;

	Vector V_a = impulse * ((float)1 / particles[0]->mass);
	particles[0]->velocity = particles[0]->velocity + V_a;

	if (particles[1]) {
		Vector V_b = impulse * ((float)1 / particles[1]->mass);
		particles[1]->velocity = particles[1]->velocity + V_b;
	}
}

void Krazy::ParticleContact::resolve(float time) {
	resolveVelocity(time);
	resolveInterpenetration(time);
}

void Krazy::ParticleContact::resolveInterpenetration(float time) {
	if (depth <= 0) return;

	float totalMass = (float)1 / particles[0]->mass;
	if (particles[1])
		totalMass += (float)1 / particles[1]->mass;

	if (totalMass <= 0) return;

	float totalMoveByMass = depth / totalMass;

	Vector moveByMass = contactNormal * totalMoveByMass;

	Vector P_a = moveByMass * ((float)1 / particles[0]->mass);
	particles[0]->position += P_a;

	if (particles[1]) {
		Vector P_b = moveByMass * (-(float)1 / particles[1]->mass);
		particles[1]->position += P_b;
	}

	depth = 0;

}