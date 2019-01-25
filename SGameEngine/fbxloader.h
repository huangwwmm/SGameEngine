#pragma once
#include "debug.h"
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettingspath.h>

class FFbxLoader
{
private:
	FbxManager *fbx_manager;

public:
	static FFbxLoader *GetInstance();

public:
	void LoadFbx(const char *filepath) const;

private:
	// UNDONE
	void ConvertToActor(const FbxNode *node) const;

private:
	FFbxLoader();
	~FFbxLoader();
};