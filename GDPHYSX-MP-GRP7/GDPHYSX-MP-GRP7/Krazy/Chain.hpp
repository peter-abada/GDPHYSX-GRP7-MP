#ifndef CHAIN_HPP
#define CHAIN_HPP

#include "ParticleLink.hpp"
#include "ParticleContact.hpp"

namespace Krazy {
	class Chain : public ParticleLink {
	public:
		float length = 1;

		float restitution = 0;

		//ParticleContact* getContact() override;
	};
}

#endif