#include "vector.h"
#include "matrix.h"
#include "ftime.h"
#include "graphics.h"
#include <iostream>
#include <intrin.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <fbxsdk.h>
#include <io.h>
#include <fcntl.h>

FILE *console_fp;
HWND hwnd;

LRESULT CALLBACK WindowProc(HWND hwnd
	, UINT message
	, WPARAM wparam
	, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}

__forceinline void CreateConsole()
{
	AllocConsole();
	freopen_s(&console_fp, "CONOUT$", "w", stdout);
}

__forceinline void DestroyConsole()
{
	fclose(console_fp);
}

__forceinline void Tick(float delta_time)
{
}

__forceinline int InfiniteGameLoop()
{
	MSG msg;
	const double dt = 1 / 30.0f;

	double currentTime = FTime::kInstance->GetCpuTime();
	double accumulator = 0.0;

	// Enter the infinite message loop
	while (true)
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
		{
			break;
		}

		double newTime = FTime::kInstance->GetCpuTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= dt)
		{
			accumulator -= dt;

			Tick(dt);
		}

		// UNDONE Render to back buffer
		Graphics::kInstance->Render();
	}

	return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance
	, HINSTANCE hPrevInstance
	, LPSTR lpCmdLine
	, int nCmdShow)
{
	CreateConsole();
	FTime::Initialize();
	Graphics::Initialize(hInstance, nCmdShow, WindowProc);

	int msg = InfiniteGameLoop();

	Graphics::Destroy();
	FTime::Destroy();
	DestroyConsole();
	return msg;
}