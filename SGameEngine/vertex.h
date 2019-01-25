#pragma once
#include "math/vector.h"

struct FVertex
{
	// moudel space
	FVector position_moudel;
	FVector normal;
	float diffuse_uv[2];
};
