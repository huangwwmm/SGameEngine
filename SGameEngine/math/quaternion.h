#pragma once
#include "math.h"
#include "rotator.h"

struct FRotator;

__declspec(align(16)) struct FQuaternion
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	__forceinline FQuaternion(float x, float y, float z, float w);
	__forceinline FQuaternion(FQuaternion& quaternion);
	__forceinline ~FQuaternion();

public:
	FRotator Rotator() const;
};

__forceinline FQuaternion::FQuaternion(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{
}

__forceinline FQuaternion::FQuaternion(FQuaternion &quaternion)
	: x(quaternion.x)
	, y(quaternion.y)
	, z(quaternion.z)
	, w(quaternion.w)
{
}

__forceinline FQuaternion::~FQuaternion()
{
}