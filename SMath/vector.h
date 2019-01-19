#pragma once
#include "smath.h"

struct Vector
{
public:
	float x;
	float y;
	float z;

public:
	static const Vector kZero;
	static const Vector kOne;

public:
	__forceinline Vector(float x, float y, float z);
	__forceinline Vector(float* arr);

	__forceinline Vector operator+(const Vector& vec) const;
	__forceinline Vector operator-(const Vector& vec) const;
	__forceinline Vector operator*(const float scale) const;
	__forceinline Vector operator/(const float scale) const;
	__forceinline bool operator==(const Vector& vec) const;
	__forceinline bool operator!=(const Vector& vec) const;
	__forceinline Vector operator+=(const Vector& vec);
	__forceinline Vector operator-=(const Vector& vec);
	__forceinline Vector operator*=(const float scale);
	__forceinline Vector operator/=(const float scale);

public:
	__forceinline void Set(float x, float y, float z);
	__forceinline void Set(float *arr);
	__forceinline float Magnitude() const;
	__forceinline float MagnitudeSquared() const;
	__forceinline void Normalize();
	__forceinline bool IsNormalize() const;

public:
	__forceinline friend std::ostream& operator<<(std::ostream& output, Vector& vec);
};

const Vector Vector::kZero(0.0f, 0.0f, 0.0f);
const Vector Vector::kOne(1.0f, 1.0f, 1.0f);

__forceinline Vector::Vector(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{
}

__forceinline Vector::Vector(float * arr)
	: x(arr[0])
	, y(arr[1])
	, z(arr[2])
{
}

__forceinline Vector Vector::operator+(const Vector & vec) const
{
#ifdef ENABLE_SEE
	return Vector((float*)&_mm_add_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	return Vector(x + vec.x, y + vec.y, z + vec.z);
#endif
}

__forceinline Vector Vector::operator-(const Vector & vec) const
{
#ifdef ENABLE_SEE
	return Vector((float*)&_mm_sub_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	return Vector(x - vec.x, y - vec.y, z - vec.z);
#endif
}

__forceinline Vector Vector::operator*(const float scale) const
{
#ifdef ENABLE_SEE
	return Vector((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(scale)));
#else
	return Vector(x * scale, y * scale, z * scale);
#endif
}

__forceinline Vector Vector::operator/(const float scale) const
{
	const float rscale = 1.0f / scale;
#ifdef ENABLE_SEE
	return Vector((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(rscale)));
#else
	return Vector(x * rscale, y * rscale, z * rscale);
#endif
}

__forceinline bool Vector::operator==(const Vector & vec) const
{
	return x == vec.x && y == vec.y && z == vec.z;
}

__forceinline bool Vector::operator!=(const Vector & vec) const
{
	return x != vec.x || y != vec.y || z != vec.z;
}

__forceinline Vector Vector::operator+=(const Vector & vec)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_add_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	x += vec.x; y += vec.y; z += vec.z;
#endif
	return *this;
}

__forceinline Vector Vector::operator-=(const Vector & vec)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_sub_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	x -= vec.x; y -= vec.y; z -= vec.z;
#endif
	return *this;
}

__forceinline Vector Vector::operator*=(const float scale)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(scale)));
#else
	x *= scale; y *= scale; z *= scale;
#endif
	return *this;
}

__forceinline Vector Vector::operator/=(const float scale)
{
	const float rscale = 1.0f / scale;
#ifdef ENABLE_SEE
	Set((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(rscale)));
#else
	x *= rscale; y *= rscale; z *= rscale;
#endif
	return *this;
}

__forceinline void Vector::Set(float x, float y, float z)
{
	this->x = x; this->y = y; this->z = z;
}

__forceinline void Vector::Set(float * arr)
{
	x = arr[0]; y = arr[1]; z = arr[2];
}

__forceinline float Vector::Magnitude() const
{
	return SMath::Sqrt(MagnitudeSquared());
}

__forceinline float Vector::MagnitudeSquared() const
{
	return x * x + y * y + z * z;
}

__forceinline void Vector::Normalize()
{
	const float scale = SMath::InvSqrt(MagnitudeSquared());
	x *= scale; y *= scale; z *= scale;
}

__forceinline bool Vector::IsNormalize() const
{
	return (SMath::Abs(1.0f - MagnitudeSquared()) < kThreshVectorNormalized);
}

__forceinline std::ostream & operator<<(std::ostream & output, Vector & vec)
{
	output << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return output;
}