#pragma once
#include "object.h"

class UWorld : UObject
{
public:
	static UWorld *GetInstance();

public:
	UWorld();
	~UWorld();
};