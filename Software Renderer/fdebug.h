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
	void AssertHresult(HRESULT hr);

public:
	FDebug();
	~FDebug();
};

#define ASSERT_HRESULT(hr) FDebug::GetInstance()->AssertHresult(hr);