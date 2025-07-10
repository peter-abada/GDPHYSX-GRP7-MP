#ifndef ROD_HPP
#define ROD_HPP

#include "ParticleLink.hpp"
#include "ParticleContact.hpp"

namespace Krazy {
	class Rod : public ParticleLink {
	public:
		float length = 1;

		float restitution = 0;

		ParticleContact* getContact() override;
	};
}

#endif