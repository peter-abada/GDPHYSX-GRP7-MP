#include "RenderParticle.hpp"

RenderParticle::RenderParticle(Krazy::PhysicsParticle* p, Model* obj) : particle(p), renderObj(obj) {
	color = Krazy::Vector(1, 1, 1);
}

RenderParticle::RenderParticle(Krazy::PhysicsParticle* p, Model* obj, Krazy::Vector c) : particle(p), renderObj(obj), color(c) {}

void RenderParticle::draw() {
	if (!particle->isDestroy()) {
		renderObj->position = (glm::vec3)particle->position;
		renderObj->draw();
	}
}