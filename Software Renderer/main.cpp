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
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = ::BeginPaint(hwnd, &ps);
		Graphics::kInstance->Draw(hdc);
		::EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
		break;
	}
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

__forceinline HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"Sample Window Class";
	RegisterClass(&wc);

	// Create the window.
	HWND hWnd = CreateWindowEx(
		0,                              // Optional window styles.
		L"Sample Window Class",         // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// Position
		Graphics::kInstance->GetWidth(), Graphics::kInstance->GetHeight(),   // Size
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hWnd == NULL)
	{
		// UNDONE Need debug library
	}

	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}

__forceinline void DestroyGameWindow(HWND hWnd)
{
	DestroyWindow(hWnd);
}

__forceinline void Tick(float delta_time)
{
}

__forceinline int InfiniteGameLoop()
{
	MSG msg;
	const double dt = 1 / 30.0f;

	double currentTime = FTime::kTime->GetCpuTime();
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

		double newTime = FTime::kTime->GetCpuTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= dt)
		{
			accumulator -= dt;

			Tick(dt);
		}

		// UNDONE Render to back buffer
		//Render();
		::InvalidateRect(hwnd, NULL, true);
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
	Graphics::Initialize(600, 480);
	hwnd = CreateGameWindow(hInstance, nCmdShow);

	int msg = InfiniteGameLoop();

	DestroyGameWindow(hwnd);
	Graphics::Destroy();
	FTime::Destroy();
	DestroyConsole();
	return msg;
}