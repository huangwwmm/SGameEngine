#pragma once
#include "macros.h"
#include <math.h>
#include <iostream>
#include <xmmintrin.h>

#pragma region const numbers
// Allowed error for a normalized vector (against squared magnitude)
const float kThreshVectorNormalized = 0.01f;
#pragma endregion

class FMath
{
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
};