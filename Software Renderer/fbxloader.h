#pragma once
#include"fdebug.h"
#include<fbxsdk.h>
#include<fbxsdk/fileio/fbxiosettingspath.h>

class FbxLoader
{
private:
	FbxManager *fbx_manager;

public:
	static FbxLoader *GetInstance();

public:
	void LoadFbx(const char *filepath) const;

private:
	// UNDONE
	void ConvertToActor(const FbxNode *node) const;

private:
	FbxLoader();
	~FbxLoader();
};