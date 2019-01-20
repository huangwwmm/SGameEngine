#pragma once
#include <cstring>

struct FMatrix
{
public:
	__declspec(align(16)) float m[4][4];

public:
	explicit __forceinline FMatrix()
	{
		memset(this, 0, sizeof(*this));
	}

	__forceinline FMatrix operator*(const FMatrix& Other) const;
};

__forceinline FMatrix FMatrix::operator*(const FMatrix & Other) const
{
	return FMatrix();
}