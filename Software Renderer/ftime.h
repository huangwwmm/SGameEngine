#pragma once
#include <windows.h>  

struct FTime
{
public:
	FTime();
	~FTime();

public:
	__forceinline double GetCpuTime();

private:
	double performance_count2millisecond;
	LARGE_INTEGER time_cache;
};

static FTime *kTime;

__forceinline FTime::FTime()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	performance_count2millisecond = 1000.0 / frequency.QuadPart;
}

__forceinline FTime::~FTime()
{
}

__forceinline double FTime::GetCpuTime()
{
	QueryPerformanceCounter(&time_cache);
	double d = time_cache.QuadPart * performance_count2millisecond;
	return time_cache.QuadPart * performance_count2millisecond;
}