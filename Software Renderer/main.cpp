#include "vector.h"
#include <iostream>
#include <intrin.h>
#include <math.h>

#define SHUFFLEMASK(A) ( (A) | ((A)<<2) | ((A)<<4) | ((A)<<6) )

int main()
{
	__m128 s = _mm_set_ps(1, 2, 3, 4);
	float* s2 = (float*)&_mm_shuffle_ps(s, s, SHUFFLEMASK(2));
	std::cout << s2[0]<<" " << s2[1] << " " << s2[2] << " " << s2[3] << " ";
}