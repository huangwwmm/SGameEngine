#pragma once
#include <windows.h>  

struct FTime
{
public:
	static FTime *kTime;

private:
	double performance_count2millisecond;
	LARGE_INTEGER time_cache;

public:
	static void Initialize();
	static void Destroy();

private:
	FTime();
	~FTime();

public:
	__forceinline double GetCpuTime()
	{
		QueryPerformanceCounter(&time_cache);
		double d = time_cache.QuadPart * performance_count2millisecond;
		return time_cache.QuadPart * performance_count2millisecond;
	}
};