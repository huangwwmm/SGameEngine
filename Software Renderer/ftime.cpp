#include "ftime.h"

FTime * FTime::GetInstance()
{
	static FTime kInstance;
	return &kInstance;
}

FTime::FTime()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	performance_count2millisecond = 1000.0 / frequency.QuadPart;
}

FTime::~FTime()
{
}