#include "game_engine.h"
#include <windows.h>

HINSTANCE kHInstance;
int kNCmdShow;

int WINAPI WinMain(HINSTANCE h_instance
	, HINSTANCE h_prev_instance
	, LPSTR lp_cmd_line
	, int n_cmd_show)
{
	kHInstance = h_instance;
	kNCmdShow = n_cmd_show;

	FGameEngine::GetInstance()->Initialize();
	int message = FGameEngine::GetInstance()->InfiniteGameLoop();
	FGameEngine::GetInstance()->Destroy();
	return message;
}