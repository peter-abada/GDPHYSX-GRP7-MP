#include "Vector.hpp"
#include <cmath>

namespace Krazy {

	Vector::Vector() : x(0), y(0), z(0) {}
	Vector::Vector(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}

	float Vector::Magnitude() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float Vector::squareMagnitude() const {
		return x * x + y * y + z * z;
	}

	Vector Vector::Normalize() const {
		float mag = Magnitude();
		if (mag == 0.0f) return Vector(0, 0, 0);
		return Vector(x / mag, y / mag, z / mag);
	}

	Vector Vector::operator+ (const Vector rhs) const {
		return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	void Vector::operator+= (const Vector rhs) {
		x += rhs.x; y += rhs.y; z += rhs.z;
	}

	Vector Vector::operator- (const Vector rhs) const {
		return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	void Vector::operator-= (const Vector rhs) {
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
	}

	Vector Vector::operator* (const float scalar) const {
		return Vector(x * scalar, y * scalar, z * scalar);
	}

	void Vector::operator*= (const float scalar) {
		x *= scalar; y *= scalar; z *= scalar;
	}

	Vector Vector::operator/ (const float scalar) const {
		return Vector(x / scalar, y / scalar, z / scalar);
	}

	void Vector::operator/= (const float scalar) {
		x /= scalar; y /= scalar; z /= scalar;
	}

	float Vector::operator, (const Vector rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vector Vector::operator& (const Vector rhs) const {
		return Vector(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		);
	}

	Vector Vector::operator% (const Vector rhs) const {
		return Vector(x * rhs.x, y * rhs.y, z * rhs.z);
	}

}
