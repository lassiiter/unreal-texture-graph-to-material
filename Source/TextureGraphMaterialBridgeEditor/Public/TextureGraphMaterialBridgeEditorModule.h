#pragma once

#include "Modules/ModuleManager.h"

class FTextureGraphMaterialBridgeMaterialCreationService;
class FTextureGraphMaterialBridgeSaveService;

class TEXTUREGRAPHMATERIALBRIDGEEDITOR_API FTextureGraphMaterialBridgeEditorModule : public IModuleInterface
{
public:
	virtual ~FTextureGraphMaterialBridgeEditorModule() override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FTextureGraphMaterialBridgeMaterialCreationService& GetMaterialCreationService();

private:
	FTextureGraphMaterialBridgeMaterialCreationService* MaterialCreationService = nullptr;
	FTextureGraphMaterialBridgeSaveService* SaveService = nullptr;
};
