#pragma once
#include "smath.h"
#include <cstring>
#include <iomanip>

struct FMatrix
{
public:
	__declspec(align(16)) float m[4][4];

public:
	static void MatrixMultiply(FMatrix *result, const  FMatrix *m1, const FMatrix *m2);

public:
	explicit __forceinline FMatrix()
	{
		memset(this, 0, sizeof(*this));
	}

	explicit __forceinline FMatrix(float *arr)
	{
		memcpy(this, arr, sizeof(*this));
	}

	__forceinline FMatrix operator*(const FMatrix &other) const;
	__forceinline void operator*=(const FMatrix &other);
	__forceinline FMatrix operator+(const FMatrix &other) const;
	__forceinline void operator+=(const FMatrix &other);
	__forceinline FMatrix operator*(const float other) const;
	__forceinline void operator*=(const float other);
	__forceinline bool operator==(const FMatrix &other);
	__forceinline bool operator!=(const FMatrix &other);

public:
	__forceinline friend std::ostream& operator<<(std::ostream &output, FMatrix &matrix);
};

__forceinline std::ostream & operator<<(std::ostream &output, FMatrix &matrix)
{
	output << std::setprecision(5) << "|" << std::setw(8) << matrix.m[0][0] << ", " << std::setw(8) << matrix.m[0][1] << ", " << std::setw(8) << matrix.m[0][2] << ", " << std::setw(8) << matrix.m[0][3] << "|\n"
		<< "|" << std::setw(8) << matrix.m[1][0] << ", " << std::setw(8) << matrix.m[1][1] << ", " << std::setw(8) << matrix.m[1][2] << ", " << std::setw(8) << matrix.m[1][3] << "|\n"
		<< "|" << std::setw(8) << matrix.m[2][0] << ", " << std::setw(8) << matrix.m[2][1] << ", " << std::setw(8) << matrix.m[2][2] << ", " << std::setw(8) << matrix.m[2][3] << "|\n"
		<< "|" << std::setw(8) << matrix.m[3][0] << ", " << std::setw(8) << matrix.m[3][1] << ", " << std::setw(8) << matrix.m[3][2] << ", " << std::setw(8) << matrix.m[3][3] << "|";

	return output;
}

__forceinline void FMatrix::MatrixMultiply(FMatrix *result, const FMatrix *m1, const FMatrix *m2)
{
	const __m128 *a = (const __m128 *)m1;
	const __m128 *b = (const __m128 *)m2;
	__m128 *r = (__m128 *)result;

	// result maybe equal m1 or m2, so cache result to r0~r3
	__m128 temp, r0, r1, r2, r3;

	// First row of result (m1[0] * m2).
	temp = _mm_mul_ps(_mm_set_ps1(a[0].m128_f32[0]), b[0]);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[0].m128_f32[1]), b[1]), temp);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[0].m128_f32[2]), b[2]), temp);
	r0 = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[0].m128_f32[3]), b[3]), temp);

	// Second row of result (m1[1] * m2).
	temp = _mm_mul_ps(_mm_set_ps1(a[1].m128_f32[0]), b[0]);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[1].m128_f32[1]), b[1]), temp);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[1].m128_f32[2]), b[2]), temp);
	r1 = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[1].m128_f32[3]), b[3]), temp);

	// Third row of result (m1[2] * m2).
	temp = _mm_mul_ps(_mm_set_ps1(a[2].m128_f32[0]), b[0]);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[2].m128_f32[1]), b[1]), temp);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[2].m128_f32[2]), b[2]), temp);
	r2 = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[2].m128_f32[3]), b[3]), temp);

	// Fourth row of result (m1[3] * m2).
	temp = _mm_mul_ps(_mm_set_ps1(a[3].m128_f32[0]), b[0]);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[3].m128_f32[1]), b[1]), temp);
	temp = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[3].m128_f32[2]), b[2]), temp);
	r3 = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(a[3].m128_f32[3]), b[3]), temp);

	// Store result
	r[0] = r0;
	r[1] = r1;
	r[2] = r2;
	r[3] = r3;
}

__forceinline FMatrix FMatrix::operator*(const FMatrix &other) const
{
	FMatrix result;
	MatrixMultiply(&result, this, &other);
	return result;
}

__forceinline void FMatrix::operator*=(const FMatrix &other)
{
	MatrixMultiply(this, this, &other);
}

__forceinline FMatrix FMatrix::operator+(const FMatrix & other) const
{
#ifdef ENABLE_SEE
	FMatrix result;
	const __m128 *a = (const __m128 *)this;
	const __m128 *b = (const __m128 *)&other;
	__m128 *r = (__m128 *)&result;

	r[0] = _mm_add_ps(a[0], b[0]);
	r[1] = _mm_add_ps(a[1], b[1]);
	r[2] = _mm_add_ps(a[2], b[2]);
	r[3] = _mm_add_ps(a[3], b[3]);

	return result;
#else
	FMatrix result;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			result.m[x][y] = m[x][y] + other.m[x][y];
		}
	}
	return result;
#endif
}

__forceinline void FMatrix::operator+=(const FMatrix & other)
{
	*this = *this + other;
}

__forceinline FMatrix FMatrix::operator*(const float other) const
{
#ifdef ENABLE_SEE
	FMatrix result;
	const __m128 *a = (const __m128 *)this;
	__m128 b = _mm_set_ps1(other);
	__m128 *r = (__m128 *)&result;

	r[0] = _mm_mul_ps(a[0], b);
	r[1] = _mm_mul_ps(a[1], b);
	r[2] = _mm_mul_ps(a[2], b);
	r[3] = _mm_mul_ps(a[3], b);

	return result;
#else
	FMatrix result;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			result.m[x][y] = m[x][y] * other;
		}
	}
	return result;
#endif
}

__forceinline void FMatrix::operator*=(const float other)
{
	*this = *this * other;
}

__forceinline bool FMatrix::operator==(const FMatrix &other)
{
#ifdef ENABLE_SEE
	const __m128 *a = (const __m128 *)this;
	const __m128 *b = (const __m128 *)&other;
	return (_mm_movemask_ps(_mm_cmpeq_ps(a[0], b[0])) & 0xF) == 0xF
		&& (_mm_movemask_ps(_mm_cmpeq_ps(a[1], b[1])) & 0xF) == 0xF
		&& (_mm_movemask_ps(_mm_cmpeq_ps(a[2], b[2])) & 0xF) == 0xF
		&& (_mm_movemask_ps(_mm_cmpeq_ps(a[3], b[3])) & 0xF) == 0xF;
#else
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (m[x][y] != other.m[x][y])
			{
				return false;
			}
		}
	}
	return true;
#endif
}

__forceinline bool FMatrix::operator!=(const FMatrix &other)
{
#ifdef ENABLE_SEE
	const __m128 *a = (const __m128 *)this;
	const __m128 *b = (const __m128 *)&other;
	return (_mm_movemask_ps(_mm_cmpneq_ps(a[0], b[0])) & 0xF) != 0x0
		|| (_mm_movemask_ps(_mm_cmpneq_ps(a[1], b[1])) & 0xF) != 0x0
		|| (_mm_movemask_ps(_mm_cmpneq_ps(a[2], b[2])) & 0xF) != 0x0
		|| (_mm_movemask_ps(_mm_cmpneq_ps(a[3], b[3])) & 0xF) != 0x0;
#else
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (m[x][y] == other.m[x][y])
			{
				return false;
			}
		}
	}
	return true;
#endif
}