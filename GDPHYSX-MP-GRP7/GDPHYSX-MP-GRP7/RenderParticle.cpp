#include "RenderParticle.hpp"

RenderParticle::RenderParticle(Physics::PhysicsParticle* p, Model* obj) : particle(p), renderObj(obj) {
	color = Physics::Vector(1, 1, 1);
}

RenderParticle::RenderParticle(Physics::PhysicsParticle* p, Model* obj, Physics::Vector c) : particle(p), renderObj(obj), color(c) {}

void RenderParticle::draw() {
	if (!particle->isDestroy()) {
		renderObj->position = (glm::vec3)particle->position;
		renderObj->draw();
	}
}