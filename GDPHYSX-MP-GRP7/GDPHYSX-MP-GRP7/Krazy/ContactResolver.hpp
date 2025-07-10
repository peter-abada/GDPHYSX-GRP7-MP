#ifndef CONTACTRESOLVER_HPP
#define CONTACTRESOLVER_HPP

#include <iostream>
#include "ParticleContact.hpp"
#include <vector>
namespace Krazy{
	class ContactResolver {
	public:
		unsigned max_iterations;

		ContactResolver(unsigned _maxIterations) : max_iterations(_maxIterations) {}

		void resolveContacts(std::vector<ParticleContact*> contacts, float time);

	protected:
		unsigned current_iterations = 0;
	};

}

#endif