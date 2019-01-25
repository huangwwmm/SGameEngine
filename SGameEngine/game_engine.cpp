#include "game_engine.h"
#include "debug.h"
#include "time.h"
#include "fbxloader.h"
#include "graphics.h"

FGameEngine *FGameEngine::GetInstance()
{
	static FGameEngine kInstance;
	return &kInstance;
}

LRESULT FGameEngine::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}

void FGameEngine::Initialize()
{
	FDebug::GetInstance();
	FTime::GetInstance();
	FFbxLoader::GetInstance();
	FGraphics::GetInstance();
}

void FGameEngine::Destroy()
{
}

int FGameEngine::InfiniteGameLoop()
{
	MSG msg;
	const double dt = 1 / 30.0f;

	double currentTime = FTime::GetInstance()->GetCpuTime();
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

		double newTime = FTime::GetInstance()->GetCpuTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= dt)
		{
			accumulator -= dt;

			Tick(dt);
		}

		FGraphics::GetInstance()->Renderer();
	}

	return (int)msg.wParam;
}

void FGameEngine::Tick(float delta_time)
{
}