#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTextureGraphMaterialBridge, Log, All);

class TEXTUREGRAPHMATERIALBRIDGE_API FTextureGraphMaterialBridgeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
