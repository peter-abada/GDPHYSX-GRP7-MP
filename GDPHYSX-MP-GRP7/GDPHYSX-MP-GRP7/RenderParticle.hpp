#ifndef RENDERPARTICLE_HPP
#define RENDERPARTICLE_HPP

#include "Krazy/PhysicsParticle.hpp"
#include "Model.hpp"

class RenderParticle {
public:
	Krazy::PhysicsParticle* particle;
	Model* renderObj;
	Krazy::Vector color;

	RenderParticle(Krazy::PhysicsParticle* p, Model* obj);

	RenderParticle(Krazy::PhysicsParticle* p, Model* obj, Krazy::Vector c);

	void draw();
};

#endif