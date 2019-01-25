#include "world.h"

UWorld *UWorld::GetInstance()
{
	static UWorld kInstance;
	return &kInstance;
}

UWorld::UWorld()
{
}

UWorld::~UWorld()
{
}