#include "TextureGraphMaterialBridgeModule.h"

#define LOCTEXT_NAMESPACE "FTextureGraphMaterialBridgeModule"

DEFINE_LOG_CATEGORY(LogTextureGraphMaterialBridge);

void FTextureGraphMaterialBridgeModule::StartupModule()
{
}

void FTextureGraphMaterialBridgeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTextureGraphMaterialBridgeModule, TextureGraphMaterialBridge)
