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
// UNDONE Drawing a class diagram of the actor lifecycle
class AActor : UObject
{
public:
	AActor();
	~AActor();
};