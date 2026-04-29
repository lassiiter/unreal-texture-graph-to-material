#include "TextureGraphMaterialBridgeEditorModule.h"

#include "TextureGraphMaterialBridgeEditorHooks.h"
#include "TextureGraphMaterialBridgeMaterialCreationService.h"
#include "TextureGraphMaterialBridgeSaveService.h"
#include "MaterialExpressionTextureGraphOutput.h"
#include "MaterialExpressionTextureGraphSample.h"
#include "SGraphNodeMaterialTextureGraphOutput.h"
#include "SGraphNodeMaterialTextureGraphSample.h"
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

TSharedPtr<SGraphNodeMaterialBase> CreateTextureGraphSampleNodeWidget(UMaterialExpressionTextureGraphSample* Expression)
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

	return SNew(SGraphNodeMaterialTextureGraphSample, MaterialGraphNode);
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
	FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphSampleNodeWidget().BindStatic(&CreateTextureGraphSampleNodeWidget);

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
	FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphSampleNodeWidget().Unbind();
}

FTextureGraphMaterialBridgeMaterialCreationService& FTextureGraphMaterialBridgeEditorModule::GetMaterialCreationService()
{
	check(MaterialCreationService);
	return *MaterialCreationService;
}

IMPLEMENT_MODULE(FTextureGraphMaterialBridgeEditorModule, TextureGraphMaterialBridgeEditor)
