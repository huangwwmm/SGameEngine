#include "ftime.h"

FTime* FTime::kTime = nullptr;

void FTime::Initialize()
{
	kTime = new FTime();
}

void FTime::Destroy()
{
	delete kTime;
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