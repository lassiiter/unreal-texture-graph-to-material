#pragma once

#include "Modules/ModuleManager.h"

class FTextureGraphMaterialBridgeMaterialCreationService;
class FTextureGraphMaterialBridgeMaterialInstanceBindingService;
class FTextureGraphMaterialBridgeSaveService;
class FTextureGraphMaterialBridgeTextureGraphCreationService;

class TEXTUREGRAPHMATERIALBRIDGEEDITOR_API FTextureGraphMaterialBridgeEditorModule : public IModuleInterface
{
public:
	virtual ~FTextureGraphMaterialBridgeEditorModule() override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FTextureGraphMaterialBridgeMaterialCreationService& GetMaterialCreationService();
	FTextureGraphMaterialBridgeMaterialInstanceBindingService& GetMaterialInstanceBindingService();
	FTextureGraphMaterialBridgeTextureGraphCreationService& GetTextureGraphCreationService();

private:
	FTextureGraphMaterialBridgeMaterialCreationService* MaterialCreationService = nullptr;
	FTextureGraphMaterialBridgeMaterialInstanceBindingService* MaterialInstanceBindingService = nullptr;
	FTextureGraphMaterialBridgeSaveService* SaveService = nullptr;
	FTextureGraphMaterialBridgeTextureGraphCreationService* TextureGraphCreationService = nullptr;
};
