#pragma once
#include "../macros.h"
#include "../debug.h"
#include <math.h>
#include <iostream>
#include <xmmintrin.h>

namespace FMath
{
#pragma region const numbers
	const float kPI = 3.1415926535897932f;
	const float kInvertPI = 0.31830988618f;
	const float kHalfPI = 1.57079632679f;
	// Note:  We use kFastAsinHalfPI instead of HALF_PI inside of FastASin(), since it was the value that accompanied the minimax coefficients below.
	// It is important to use exactly the same value in all places inside this function to ensure that FastAsin(0.0f) == 0.0f.
	// For comparison:
	//		kHalfPI				== 1.57079632679f == 0x3fC90FDB
	//		kFastAsinHalfPI		== 1.5707963050f  == 0x3fC90FDA
	const float kFastAsinHalfPI = 1.5707963050f;
	const float kSingularityThreshold = 0.4999995f;
	const float kRad2Deg = (180.f) / FMath::kPI;
	// Allowed error for a normalized vector (against squared magnitude)
	const float kThreshVectorNormalized = 0.01f;
#pragma endregion

	static __forceinline float InvSqrt(float value)
	{
#ifdef ENABLE_SEE
		// Performs two passes of Newton-Raphson iteration on the hardware estimate
		//    v^-0.5 = x
		// => x^2 = v^-1
		// => 1/(x^2) = v
		// => F(x) = x^-2 - v
		//    F'(x) = -2x^-3

		//    x1 = x0 - F(x0)/F'(x0)
		// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
		// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
		// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
		//
		// This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
		// but retains better accuracy (namely InvSqrt(1) = 1 exactly).

		const __m128 one_half = _mm_set_ss(0.5f);
		__m128 y0, x0, x1, x2, over2;
		float result;

		y0 = _mm_set_ss(value);
		x0 = _mm_rsqrt_ss(y0);	// 1/sqrt estimate (12 bits)
		over2 = _mm_mul_ss(y0, one_half);

		// 1st Newton-Raphson iteration
		x1 = _mm_mul_ss(x0, x0);
		x1 = _mm_sub_ss(one_half, _mm_mul_ss(over2, x1));
		x1 = _mm_add_ss(x0, _mm_mul_ss(x0, x1));

		// 2nd Newton-Raphson iteration
		x2 = _mm_mul_ss(x1, x1);
		x2 = _mm_sub_ss(one_half, _mm_mul_ss(over2, x2));
		x2 = _mm_add_ss(x1, _mm_mul_ss(x1, x2));

		_mm_store_ss(&result, x2);
		return result;
#else
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = value * 0.5F;
		y = value;
		i = *(long *)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);             // what the fuck? 
		y = *(float *)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
		y = y * (threehalfs - (x2 * y * y));   // 2nd iteration

		return y;
#endif
	}

	static __forceinline float Abs(const float value)
	{
		return fabsf(value);
	}

	static __forceinline float Sqrt(float value)
	{
		return sqrtf(value);
	}

	static __forceinline float Square(float value)
	{
		return value * value;
	}

	static float Atan2(float y, float x)
	{
		//return atan2f(Y,X);
		// atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
		// We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
		// On PC this has been measured to be 2x faster than the std C version.

		const float abs_x = FMath::Abs(x);
		const float abs_y = FMath::Abs(y);
		const bool abs_y_bigger = (abs_y > abs_x);
		float abs_max = abs_y_bigger ? abs_y : abs_x; // Max(abs_x, abs_y)
		const float abs_min = abs_y_bigger ? abs_x : abs_y; // Min(abs_x, abs_y)

		if (abs_max == 0.f)
		{
			return 0.f;
		}

		float t3 = abs_min / abs_max;
		const float t4 = t3 * t3;

		static const float c[7] = {
			+7.2128853633444123e-03f,
			-3.5059680836411644e-02f,
			+8.1675882859940430e-02f,
			-1.3374657325451267e-01f,
			+1.9856563505717162e-01f,
			-3.3324998579202170e-01f,
			+1.0f
		};

		abs_max = c[0];
		abs_max = abs_max * t4 + c[1];
		abs_max = abs_max * t4 + c[2];
		abs_max = abs_max * t4 + c[3];
		abs_max = abs_max * t4 + c[4];
		abs_max = abs_max * t4 + c[5];
		abs_max = abs_max * t4 + c[6];
		t3 = abs_max * t3;

		t3 = abs_y_bigger ? (0.5f * kPI) - t3 : t3;
		t3 = (x < 0.0f) ? kPI - t3 : t3;
		t3 = (y < 0.0f) ? -t3 : t3;

		return t3;
	}

	// Computes the ASin of a scalar value.
	static __forceinline float FastAsin(float Value)
	{
		// Clamp input to [-1,1].
		const bool nonnegative = (Value >= 0.0f);
		const float x = FMath::Abs(Value);
		float omx = 1.0f - x;
		if (omx < 0.0f)
		{
			omx = 0.0f;
		}
		const float root = FMath::Sqrt(omx);
		// 7-degree minimax approximation
		float result = ((((((-0.0012624911f * x + 0.0066700901f)
			* x - 0.0170881256f)
			* x + 0.0308918810f)
			* x - 0.0501743046f)
			* x + 0.0889789874f)
			* x - 0.2145988016f)
			* x + kFastAsinHalfPI;
		result *= root;  // acos(|x|)
		// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
		return (nonnegative ? kFastAsinHalfPI - result : result - kFastAsinHalfPI);
	}

	// Converts a float to an integer with truncation towards zero.
	static __forceinline int TruncToInt(float f)
	{
		return (int)f;
	}

	// Converts a float to an integer value with truncation towards zero.
	static __forceinline float TruncToFloat(float f)
	{
		return (float)TruncToInt(f);
	}

	// Returns the floating-point remainder of X / Y
	// Warning: Always returns remainder toward 0, not toward the smaller multiple of Y.
	//			So for example Fmod(2.8f, 2) gives .8f as you would expect, however, Fmod(-2.8f, 2) gives -.8f, NOT 1.2f
	// Use Floor instead when snapping positions that can be negative to a grid
	static __forceinline float Fmod(float x, float y)
	{
		if (fabsf(y) <= 1.e-8f)
		{
			FLOG_ERROR("FMath::Fmod: y is zero, this is invalid and would result in NaN!");
			return 0.f;
		}

		const float quotient = TruncToFloat(x / y);
		float int_portion = y * quotient;

		// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
		// For example Fmod(55.8, 9.3) would result in a very small negative value!
		if (fabsf(int_portion) > fabsf(x))
		{
			int_portion = x;
		}

		return x - int_portion;
	}
};