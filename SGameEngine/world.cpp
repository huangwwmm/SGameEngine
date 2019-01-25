#include "world.h"

FWorld *FWorld::GetInstance()
{
	static FWorld kInstance;
	return &kInstance;
}

FWorld::FWorld()
{
}

FWorld::~FWorld()
{
}