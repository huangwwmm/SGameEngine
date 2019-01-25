#pragma once
#include <windows.h>  

struct FTime
{
private:
	double performance_count2millisecond;
	LARGE_INTEGER time_cache;

public:
	static FTime *GetInstance();

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