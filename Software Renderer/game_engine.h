#pragma once
#include <windows.h>

class FGameEngine
{
public:
	static FGameEngine *GetInstance();
	static LRESULT CALLBACK WindowProc(HWND hwnd
		, UINT message
		, WPARAM wparam
		, LPARAM lparam);

public:
	void Initialize();
	void Destroy();

public:
	int InfiniteGameLoop();

private:
	void Tick(float delta_time);
};