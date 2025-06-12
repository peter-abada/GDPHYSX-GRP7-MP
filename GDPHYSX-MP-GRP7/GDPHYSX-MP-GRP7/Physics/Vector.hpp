#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "glm/glm.hpp"

namespace Physics {
	class Vector {
	public:
		float x, y, z;

		Vector();
		Vector(const float _x, const float _y, const float _z);

		explicit operator glm::vec3() const { return glm::vec3(x, y, z); }

		float Magnitude();

		//Direction via normalized vector
		Vector Normalize();

		Vector operator+ (const Vector rhs);

		void operator+= (const Vector rhs);

		Vector operator- (const Vector rhs);

		void operator-= (const Vector rhs);

		//Scalar multiplication
		Vector operator* (const float scalar);

		void operator*= (const float scalar);

		Vector operator/ (const float scalar);

		void operator/= (const float scalar);

		//Dot product (can't overload '.')
		float operator, (const Vector rhs);

		//Cross product
		Vector operator& (const Vector rhs);

		//Component product
		Vector operator% (const Vector rhs);
	};
}

#endif