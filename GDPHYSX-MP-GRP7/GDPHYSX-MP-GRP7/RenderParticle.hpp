#ifndef RENDERPARTICLE_HPP
#define RENDERPARTICLE_HPP

#include "Physics/PhysicsParticle.hpp"
#include "Model.hpp"

class RenderParticle {
public:
	Physics::PhysicsParticle* particle;
	Model* renderObj;
	Physics::Vector color;

	RenderParticle(Physics::PhysicsParticle* p, Model* obj);

	RenderParticle(Physics::PhysicsParticle* p, Model* obj, Physics::Vector c);

	void draw();
};

#endif