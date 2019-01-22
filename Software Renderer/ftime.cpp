#include "ftime.h"

FTime* FTime::kInstance = nullptr;

void FTime::Initialize()
{
	kInstance = new FTime();
}

void FTime::Destroy()
{
	delete kInstance;
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