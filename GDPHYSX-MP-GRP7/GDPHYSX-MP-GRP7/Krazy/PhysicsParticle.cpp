#include "PhysicsParticle.hpp"



void Krazy::PhysicsParticle::updatePosition(float time) {
	//position = position + velocity * time;
	this->position = this->position + (this->velocity * time) + (this->accel * time * time) * ((1.0f / 2.0f));
}

void Krazy::PhysicsParticle::update(float time) {
	this->updatePosition(time);
	this->updateVelocity(time);

	this->resetForce();
}

void Krazy::PhysicsParticle::updateVelocity(float time) {
	this->accel += accumulatedForce * (1 / mass);
	
	this->velocity = this->velocity + (this->accel * time);
	this->velocity = this->velocity * powf(damping, time);
}

void Krazy::PhysicsParticle::destroy() {
	this->isDestroyed = true;
}

bool Krazy::PhysicsParticle::isDestroy() {
	return isDestroyed;
}

void Krazy::PhysicsParticle::addForce(Krazy::Vector force) {
	this->accumulatedForce += force;
}

void Krazy::PhysicsParticle::resetForce() {
	this->accumulatedForce = Krazy::Vector(0, 0, 0);
	this->accel = Krazy::Vector(0, 0, 0);
}