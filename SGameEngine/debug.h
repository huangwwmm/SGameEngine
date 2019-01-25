#pragma once
#include <windows.h>
#include <sstream>
#include <consoleapi.h>
#include <comdef.h>
#include <iostream>

class FDebug
{
private:
	FILE *console_in_fp;
	FILE *console_out_fp;
	FILE *console_err_fp;

public:
	static FDebug *GetInstance();

public:
	void AssertHresult(HRESULT hr) const;
	void Assert(const char *message) const;

public:
	FDebug();
	~FDebug();
};

#define FASSERT_HRESULT(hr) FDebug::GetInstance()->AssertHresult(hr);
#define FASSERT(valid, message) if(!(valid)) {FDebug::GetInstance()->Assert(message);}
#define FLOG(logs) std::cout<<logs;