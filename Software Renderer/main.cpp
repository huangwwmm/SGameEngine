#include "vector.h"
#include "matrix.h"
#include "ftime.h"
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
HWND hWnd;

LRESULT CALLBACK WindowProc(HWND hWnd
	, UINT message
	, WPARAM wparam
	, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = ::BeginPaint(hWnd, &ps);
		// UNDONE read pixel color from front buffer
		for (int i = 0; i < 400; i++)
		{
			for (int j = 0; j < 50; j++)
				SetPixel(hdc, i, j, 0x561);
		}
		::EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wparam, lparam);
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
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Size and position
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

	double currentTime = kTime->GetCpuTime();
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

		double newTime = kTime->GetCpuTime();
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
		::InvalidateRect(hWnd, NULL, true);
	}

	return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance
	, HINSTANCE hPrevInstance
	, LPSTR lpCmdLine
	, int nCmdShow)
{
	// TEST Is fbxsdk linked success?
	{
		FbxManager* mManager = FbxManager::Create();
		FbxIOSettings* ios = FbxIOSettings::Create(mManager, IOSROOT);
		ios->SetBoolProp(IMP_ANIMATION, false);
		ios->SetBoolProp(IMP_SETLOCKEDATTRIB, false);
		mManager->SetIOSettings(ios);
	}

	CreateConsole();
	hWnd = CreateGameWindow(hInstance, nCmdShow);
	kTime = new FTime();

	int msg = InfiniteGameLoop();

	delete kTime;
	DestroyGameWindow(hWnd);
	DestroyConsole();
	return msg;
}