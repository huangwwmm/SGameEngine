#pragma once
#include "macros.h"
#include <math.h>
#include <iostream>
#include <xmmintrin.h>

#pragma region const numbers
// Allowed error for a normalized vector (against squared magnitude)
const float kThreshVectorNormalized = 0.01f;
#pragma endregion

namespace SMath
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
		return 1.0f / sqrt(value);
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