#pragma once
#include "math.h"

__declspec(align(16)) struct FVector
{
public:
	float x;
	float y;
	float z;

public:
	static const FVector kZero;
	static const FVector kOne;

public:
	__forceinline static FVector Cross(const FVector &a, const FVector &b);
	__forceinline static float Dot(const FVector &a, const FVector &b);

public:
	__forceinline FVector(float x, float y, float z);
	explicit __forceinline FVector(float *arr);

	__forceinline FVector operator+(const FVector &vec) const;
	__forceinline FVector operator-(const FVector &vec) const;
	__forceinline FVector operator*(const float scale) const;
	__forceinline FVector operator/(const float scale) const;
	__forceinline bool operator==(const FVector &vec) const;
	__forceinline bool operator!=(const FVector &vec) const;
	__forceinline FVector operator+=(const FVector &vec);
	__forceinline FVector operator-=(const FVector &vec);
	__forceinline FVector operator*=(const float scale);
	__forceinline FVector operator/=(const float scale);

public:
	__forceinline void Set(float x, float y, float z);
	__forceinline void Set(float *arr);
	__forceinline float Magnitude() const;
	__forceinline float MagnitudeSquared() const;
	__forceinline void Normalize();
	__forceinline bool IsNormalize() const;

public:
	__forceinline friend std::ostream& operator<<(std::ostream &output, FVector &vec);
};

const FVector FVector::kZero(0.0f, 0.0f, 0.0f);
const FVector FVector::kOne(1.0f, 1.0f, 1.0f);

__forceinline FVector FVector::Cross(const FVector &a, const FVector &b)
{
	return FVector
	(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

__forceinline float FVector::Dot(const FVector &a, const FVector &b)
{
#ifdef ENABLE_SEE
	float *result = (float*)&_mm_mul_ps(_mm_load_ps(&(a.x)), _mm_load_ps(&(b.x)));
	return result[0] + result[1] + result[2];
#else
	return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

__forceinline FVector::FVector(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{
}

__forceinline FVector::FVector(float *arr)
{
	memcpy(this, arr, sizeof(FVector));
}

__forceinline FVector FVector::operator+(const FVector &vec) const
{
#ifdef ENABLE_SEE
	return FVector((float*)&_mm_add_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	return Vector(x + vec.x, y + vec.y, z + vec.z);
#endif
}

__forceinline FVector FVector::operator-(const FVector &vec) const
{
#ifdef ENABLE_SEE
	return FVector((float*)&_mm_sub_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	return Vector(x - vec.x, y - vec.y, z - vec.z);
#endif
}

__forceinline FVector FVector::operator*(const float scale) const
{
#ifdef ENABLE_SEE
	return FVector((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(scale)));
#else
	return Vector(x * scale, y * scale, z * scale);
#endif
}

__forceinline FVector FVector::operator/(const float scale) const
{
	const float rscale = 1.0f / scale;
#ifdef ENABLE_SEE
	return FVector((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(rscale)));
#else
	return Vector(x * rscale, y * rscale, z * rscale);
#endif
}

__forceinline bool FVector::operator==(const FVector &vec) const
{
#ifdef ENABLE_SEE
	return (_mm_movemask_ps(_mm_cmpeq_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x))) & 0x7) == 0x7;
#else
	return x == vec.x && y == vec.y && z == vec.z;
#endif
}

__forceinline bool FVector::operator!=(const FVector &vec) const
{
#ifdef ENABLE_SEE
	return (_mm_movemask_ps(_mm_cmpneq_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x))) & 0x7) != 0x0;
#else
	return x != vec.x || y != vec.y || z != vec.z;
#endif
}

__forceinline FVector FVector::operator+=(const FVector &vec)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_add_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	x += vec.x; y += vec.y; z += vec.z;
#endif
	return *this;
}

__forceinline FVector FVector::operator-=(const FVector &vec)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_sub_ps(_mm_load_ps(&x), _mm_load_ps(&vec.x)));
#else
	x -= vec.x; y -= vec.y; z -= vec.z;
#endif
	return *this;
}

__forceinline FVector FVector::operator*=(const float scale)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(scale)));
#else
	x *= scale; y *= scale; z *= scale;
#endif
	return *this;
}

__forceinline FVector FVector::operator/=(const float scale)
{
	const float rscale = 1.0f / scale;
#ifdef ENABLE_SEE
	Set((float*)&_mm_mul_ps(_mm_load_ps(&x), _mm_set_ps1(rscale)));
#else
	x *= rscale; y *= rscale; z *= rscale;
#endif
	return *this;
}

__forceinline void FVector::Set(float x, float y, float z)
{
	this->x = x; this->y = y; this->z = z;
}

__forceinline void FVector::Set(float *arr)
{
	memcpy(this, arr, sizeof(FVector));
}

__forceinline float FVector::Magnitude() const
{
	return FMath::Sqrt(MagnitudeSquared());
}

__forceinline float FVector::MagnitudeSquared() const
{
	return x * x + y * y + z * z;
}

__forceinline void FVector::Normalize()
{
	const float scale = FMath::InvSqrt(MagnitudeSquared());
	x *= scale; y *= scale; z *= scale;
}

__forceinline bool FVector::IsNormalize() const
{
	return (FMath::Abs(1.0f - MagnitudeSquared()) < FMath::kThreshVectorNormalized);
}

__forceinline std::ostream & operator<<(std::ostream &output, FVector &vec)
{
	output << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return output;
}