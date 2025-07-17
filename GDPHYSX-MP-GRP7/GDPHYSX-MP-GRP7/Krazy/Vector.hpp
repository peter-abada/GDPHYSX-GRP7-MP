#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "glm/glm.hpp"

namespace Krazy {
	class Vector {
	public:
		float x, y, z;

		Vector();
		Vector(const float _x, const float _y, const float _z);

		explicit operator glm::vec3() const { return glm::vec3(x, y, z); }

		float Magnitude() const;

		float squareMagnitude() const;

		//Direction via normalized vector
		Vector Normalize() const;

		Vector operator+ (const Vector rhs) const;

		void operator+= (const Vector rhs);

		Vector operator- (const Vector rhs) const;

		void operator-= (const Vector rhs);

		//Scalar multiplication
		Vector operator* (const float scalar) const;

		void operator*= (const float scalar);

		Vector operator/ (const float scalar) const;

		void operator/= (const float scalar);

		//Dot product (can't overload '.')
		float operator, (const Vector rhs) const;

		//Cross product
		Vector operator& (const Vector rhs) const;

		//Component product
		Vector operator% (const Vector rhs) const;
	};
}

#endif
