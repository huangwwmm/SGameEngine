#pragma once
#include "actor_component.h"
#include "math/vector.h"

class UScreenComponent : UActorComponent
{
public:
	FVector location;

public:
	UScreenComponent();
	~UScreenComponent();
};