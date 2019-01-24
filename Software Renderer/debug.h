#pragma once
#include <windows.h>
#include <sstream>
#include <consoleapi.h>
#include <comdef.h>
#include <iostream>

class FDebug
{
private:
	FILE *console_fp;

public:
	static FDebug *GetInstance();

public:
	void AssertHresult(HRESULT hr) const;
	void Assert(const char *message) const;

public:
	FDebug();
	~FDebug();
};

#define ASSERT_HRESULT(hr) FDebug::GetInstance()->AssertHresult(hr);
#define ASSERT(valid, message) if(!valid) {FDebug::GetInstance()->Assert(message);}