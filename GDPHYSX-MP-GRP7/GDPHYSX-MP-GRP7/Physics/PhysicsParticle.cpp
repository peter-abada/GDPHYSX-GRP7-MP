#include "PhysicsParticle.hpp"



void Physics::PhysicsParticle::updatePosition(float time) {
	//position = position + velocity * time;
	this->position = this->position + (this->velocity * time) + (this->accel * time * time) * ((1.0f / 2.0f));
}

void Physics::PhysicsParticle::update(float time) {
	this->updatePosition(time);
	this->updateVelocity(time);

	this->resetForce();
}

void Physics::PhysicsParticle::updateVelocity(float time) {
	this->accel += accumulatedForce * (1 / mass);
	
	this->velocity = this->velocity + (this->accel * time);
	this->velocity = this->velocity * powf(damping, time);
}

void Physics::PhysicsParticle::destroy() {
	this->isDestroyed = true;
}

bool Physics::PhysicsParticle::isDestroy() {
	return isDestroyed;
}

void Physics::PhysicsParticle::addForce(Physics::Vector force) {
	this->accumulatedForce += force;
}

void Physics::PhysicsParticle::resetForce() {
	this->accumulatedForce = Physics::Vector(0, 0, 0);
	this->accel = Physics::Vector(0, 0, 0);
}