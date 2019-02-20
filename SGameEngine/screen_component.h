#pragma once
#include "actor_component.h"
#include "math/vector.h"
#include "math/rotator.h"

class UScreenComponent : UActorComponent
{
public:
	FVector location;
	FRotator rotation;
	FVector scale;

public:
	UScreenComponent();
	~UScreenComponent();
};