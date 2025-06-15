#include "Vector.hpp"

Krazy::Vector::Vector() : x(0), y(0), z(0) {}

Krazy::Vector::Vector(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}

Krazy::Vector Krazy::Vector::operator+ (const Vector rhs) {
	Vector sum(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);

	return sum;
}

float Krazy::Vector::Magnitude() {
	return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}

//Direction via normalized vector
Krazy::Vector Krazy::Vector::Normalize() {
	Vector dir(this->x / Magnitude(), this->y / Magnitude(), this->z / Magnitude());

	return dir;
}

void Krazy::Vector::operator+= (const Vector rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
}

Krazy::Vector Krazy::Vector::operator- (const Vector rhs) {
	Vector dif(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);

	return dif;
}

void Krazy::Vector::operator-= (const Vector rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
}

//Scalar multiplication
Krazy::Vector Krazy::Vector::operator* (const float scalar) {
	Vector prod(this->x * scalar, this->y * scalar, this->z * scalar);

	return prod;
}

void Krazy::Vector::operator*= (const float scalar) {
	this->x *= scalar;
	this->y *= scalar;
	this->z *= scalar;
}

Krazy::Vector Krazy::Vector::operator/ (const float scalar) {
	Vector prod(this->x / scalar, this->y / scalar, this->z / scalar);

	return prod;
}

void Krazy::Vector::operator/= (const float scalar) {
	this->x /= scalar;
	this->y /= scalar;
	this->z /= scalar;
}

//Dot product (can't overload '.')
float Krazy::Vector::operator,(const Vector rhs) {
	float prod = (this->x * rhs.x) + (this->y * rhs.y) + (this->z * rhs.z);

	return prod;
}

//Cross product
Krazy::Vector Krazy::Vector::operator&(const Vector rhs) {
	Vector prod((this->y * rhs.z) - (this->z * rhs.y), 
				(this->z * rhs.x) - (this->x * rhs.z), 
				(this->x * rhs.y) - (this->y * rhs.x));

	return prod;
}

//Component product
Krazy::Vector Krazy::Vector::operator%(const Vector rhs) {
	Vector prod(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);

	return prod;
}