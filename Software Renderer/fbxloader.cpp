#include "fbxloader.h"

FbxLoader *FbxLoader::GetInstance()
{
	static FbxLoader kInstance;
	return &kInstance;
}

void FbxLoader::LoadFbx(const char * filepath) const
{
	// Create an importer.
	FbxImporter *importer = FbxImporter::Create(fbx_manager, "");

	bool import_state = importer->Initialize(filepath
		, -1
		, fbx_manager->GetIOSettings());
	ASSERT(import_state, importer->GetStatus().GetErrorString());

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(fbx_manager, "");
	// Import the contents of the file into the scene.
	importer->Import(scene);
	importer->Destroy();

	FbxNode *root_node = scene->GetRootNode();
	ConvertToActor(root_node);
}

void FbxLoader::ConvertToActor(const FbxNode *node) const
{
	int child_count = node->GetChildCount();
	for (int i_child = 0; i_child < child_count; i_child++)
	{
		const FbxNode* iter_node = node->GetChild(i_child);
		const FbxNodeAttribute *attribute = iter_node->GetNodeAttribute();
		FbxNodeAttribute::EType etype = attribute->GetAttributeType();

		if (iter_node->GetChildCount() > 0)
		{
			ConvertToActor(iter_node);
		}
	}
}

FbxLoader::FbxLoader()
{
	// Create the FBX SDK manager
	fbx_manager = FbxManager::Create();
	ASSERT(fbx_manager, "FbxManager::Create");

	// Create an IOSettings object
	FbxIOSettings *io_setting = FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(io_setting);
}

FbxLoader::~FbxLoader()
{
	fbx_manager->Destroy();
}