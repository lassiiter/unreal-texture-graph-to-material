#pragma once

#include "Modules/ModuleManager.h"

class FTextureGraphMaterialBridgeMaterialCreationService;
class FTextureGraphMaterialBridgeMaterialInstanceBindingService;
class FTextureGraphMaterialBridgeSaveService;

class TEXTUREGRAPHMATERIALBRIDGEEDITOR_API FTextureGraphMaterialBridgeEditorModule : public IModuleInterface
{
public:
	virtual ~FTextureGraphMaterialBridgeEditorModule() override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FTextureGraphMaterialBridgeMaterialCreationService& GetMaterialCreationService();
	FTextureGraphMaterialBridgeMaterialInstanceBindingService& GetMaterialInstanceBindingService();

private:
	FTextureGraphMaterialBridgeMaterialCreationService* MaterialCreationService = nullptr;
	FTextureGraphMaterialBridgeMaterialInstanceBindingService* MaterialInstanceBindingService = nullptr;
	FTextureGraphMaterialBridgeSaveService* SaveService = nullptr;
};
