#pragma once
#include "object.h"

// The actor is the smallest unit in the world
//
// Q: How to spawn an actor?
// A: By the following method:
//		UNDONE Call the function in UWorld to spawn actor
//		UNDONE UWorld auto spawn actor when load level  
//			My plan level data is a tree structure
//			ActorSpawner type nodes are to spawn actor
// 
// Workflow:
// |OnSpawned: be called at the frame begin if spawn from level
// |OnActivated: be called after all actor BeginPlay
// |Tick: by group if actor is allow tick and has group flag
// |OnDeactivated: be called at the frame end 
// |OnDestroy:
class AActor : UObject
{
public:
	AActor();
	~AActor();

public:
	virtual void OnSpawned() = 0;
	virtual void OnActivated() = 0;
	virtual void Tick(const float delta) = 0;
	virtual void OnDeactivated() = 0;
	virtual void OnDestroy() = 0;
};