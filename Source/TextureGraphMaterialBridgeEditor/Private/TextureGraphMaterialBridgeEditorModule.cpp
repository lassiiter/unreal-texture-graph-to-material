#include "TextureGraphMaterialBridgeEditorModule.h"

#include "TextureGraphMaterialBridgeEditorHooks.h"
#include "TextureGraphMaterialBridgeMaterialCreationService.h"
#include "TextureGraphMaterialBridgeSaveService.h"
#include "MaterialExpressionTextureGraphOutput.h"
#include "SGraphNodeMaterialTextureGraphOutput.h"
#include "MaterialGraph/MaterialGraphNode.h"

namespace
{
TSharedPtr<SGraphNodeMaterialBase> CreateTextureGraphOutputNodeWidget(UMaterialExpressionTextureGraphOutput* Expression)
{
	if (!Expression)
	{
		return nullptr;
	}

	UMaterialGraphNode* MaterialGraphNode = Cast<UMaterialGraphNode>(Expression->GetEditorGraphNode());
	if (!MaterialGraphNode)
	{
		return nullptr;
	}

	return SNew(SGraphNodeMaterialTextureGraphOutput, MaterialGraphNode);
}
}

FTextureGraphMaterialBridgeEditorModule::~FTextureGraphMaterialBridgeEditorModule()
{
	delete SaveService;
	SaveService = nullptr;

	delete MaterialCreationService;
	MaterialCreationService = nullptr;
}

void FTextureGraphMaterialBridgeEditorModule::StartupModule()
{
	FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphOutputNodeWidget().BindStatic(&CreateTextureGraphOutputNodeWidget);

	MaterialCreationService = new FTextureGraphMaterialBridgeMaterialCreationService();

	SaveService = new FTextureGraphMaterialBridgeSaveService();
	SaveService->Startup();
}

void FTextureGraphMaterialBridgeEditorModule::ShutdownModule()
{
	if (SaveService)
	{
		SaveService->Shutdown();
		delete SaveService;
		SaveService = nullptr;
	}

	delete MaterialCreationService;
	MaterialCreationService = nullptr;

	FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphOutputNodeWidget().Unbind();
}

FTextureGraphMaterialBridgeMaterialCreationService& FTextureGraphMaterialBridgeEditorModule::GetMaterialCreationService()
{
	check(MaterialCreationService);
	return *MaterialCreationService;
}

IMPLEMENT_MODULE(FTextureGraphMaterialBridgeEditorModule, TextureGraphMaterialBridgeEditor)
