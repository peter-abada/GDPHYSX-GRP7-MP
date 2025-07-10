#ifndef BUNGEE_HPP
#define BUNGEE_HPP

#include "ParticleLink.hpp"
#include "ParticleContact.hpp"

namespace Krazy {
	class Bungee : public ParticleLink {
	public:
		float length = 1;

		float restitution = 1;

		//ParticleContact* getContact() override;
	};
}

#endif