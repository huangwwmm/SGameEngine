#pragma once
#include "math.h"

// All rotation values are stored in degrees.
__declspec(align(16)) struct FRotator
{
public:
	static const FRotator kZero;

public:
	// Rotation around the right axis (around Y axis), Looking up and down (0=Straight Ahead, +Up, -Down)
	float pitch;
	// Rotation around the up axis (around Z axis), Running in circles 0=East, +North, -South.
	float yaw;
	// Rotation around the forward axis (around X axis), Tilting your head, 0=Straight, +Clockwise, -CCW.
	float roll;

public:
	__forceinline FRotator(float pitch, float yaw, float roll);
	explicit __forceinline FRotator(float *arr);

	__forceinline FRotator operator+(const FRotator &rot) const;
	__forceinline FRotator operator-(const FRotator &rot) const;
	__forceinline FRotator operator*(const float scale) const;
	__forceinline FRotator operator/(const float scale) const;
	__forceinline bool operator==(const FRotator &rot) const;
	__forceinline bool operator!=(const FRotator &rot) const;
	__forceinline FRotator operator+=(const FRotator &rot);
	__forceinline FRotator operator-=(const FRotator &rot);
	__forceinline FRotator operator*=(const float scale);
	__forceinline FRotator operator/=(const float scale);

public:
	__forceinline void Set(float pitch, float yaw, float roll);
	__forceinline void Set(float *arr);
};

const FRotator FRotator::kZero(0.0f, 0.0f, 0.0f);

__forceinline FRotator::FRotator(float pitch, float yaw, float roll)
	: pitch(pitch)
	, yaw(yaw)
	, roll(roll)
{
}

__forceinline FRotator::FRotator(float *arr)
{
	memcpy(this, arr, sizeof(*this));
}

__forceinline FRotator FRotator::operator+(const FRotator & rot) const
{
#ifdef ENABLE_SEE
	return FRotator((float*)&_mm_add_ps(_mm_load_ps(&pitch), _mm_load_ps(&rot.pitch)));
#else
	return FRotator(pitch + rot.pitch, yaw + rot.yaw, rot + rot.rot);
#endif
}

__forceinline FRotator FRotator::operator-(const FRotator & rot) const
{
#ifdef ENABLE_SEE
	return FRotator((float*)&_mm_sub_ps(_mm_load_ps(&pitch), _mm_load_ps(&rot.pitch)));
#else
	return FRotator(pitch - rot.pitch, yaw - rot.yaw, roll - rot.roll);
#endif
}

__forceinline FRotator FRotator::operator*(const float scale) const
{
#ifdef ENABLE_SEE
	return FRotator((float*)&_mm_sub_ps(_mm_load_ps(&pitch), _mm_set_ps1(scale)));
#else
	return FRotator(pitch * scale, yaw * scale, roll * scale);
#endif
}

__forceinline FRotator FRotator::operator/(const float scale) const
{
	const float rscale = 1.0f / scale;
#ifdef ENABLE_SEE
	return FRotator((float*)&_mm_sub_ps(_mm_load_ps(&pitch), _mm_set_ps1(rscale)));
#else
	return FRotator(pitch * rscale, yaw * rscale, roll * rscale);
#endif
}

__forceinline bool FRotator::operator==(const FRotator & rot) const
{
#ifdef ENABLE_SEE
	return (_mm_movemask_ps(_mm_cmpeq_ps(_mm_load_ps(&pitch), _mm_load_ps(&rot.pitch))) & 0x7) == 0x7;
#else
	return pitch == rot.pitch && yaw == rot.yaw && roll == rot.roll;
#endif
}

__forceinline bool FRotator::operator!=(const FRotator & rot) const
{
#ifdef ENABLE_SEE
	return (_mm_movemask_ps(_mm_cmpneq_ps(_mm_load_ps(&pitch), _mm_load_ps(&rot.pitch))) & 0x7) != 0x0;
#else
	return pitch != rot.pitch || yaw != rot.yaw || roll != rot.roll;
#endif
}

__forceinline FRotator FRotator::operator+=(const FRotator & rot)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_add_ps(_mm_load_ps(&pitch), _mm_load_ps(&rot.pitch)));
#else
	pitch += rot.pitch; yaw += rot.yaw; roll += rot.roll;
#endif
	return *this;
}

__forceinline FRotator FRotator::operator-=(const FRotator & rot)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_sub_ps(_mm_load_ps(&pitch), _mm_load_ps(&rot.pitch)));
#else
	pitch -= rot.pitch; yaw -= rot.yaw; roll -= rot.roll;
#endif
	return *this;
}

__forceinline FRotator FRotator::operator*=(const float scale)
{
#ifdef ENABLE_SEE
	Set((float*)&_mm_mul_ps(_mm_load_ps(&pitch), _mm_set_ps1(scale)));
#else
	pitch *= rot.pitch; yaw *= rot.yaw; roll *= rot.roll;
#endif
}

__forceinline FRotator FRotator::operator/=(const float scale)
{
	const float rscale = 1.0f / scale;
#ifdef ENABLE_SEE
	Set((float*)&_mm_mul_ps(_mm_load_ps(&pitch), _mm_set_ps1(rscale)));
#else
	pitch *= rscale; yaw *= rscale; roll *= rscale;
#endif
}

__forceinline void FRotator::Set(float pitch, float yaw, float roll)
{
	this->pitch = pitch; this->yaw = yaw; this->roll = roll;
}

__forceinline void FRotator::Set(float *arr)
{
	memcpy(this, arr, 16);
}