#include "TextureGraphMaterialBridgeModule.h"

#if TGMB_WITH_DESIGNER_NODES
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#endif

#define LOCTEXT_NAMESPACE "FTextureGraphMaterialBridgeModule"

DEFINE_LOG_CATEGORY(LogTextureGraphMaterialBridge);

void FTextureGraphMaterialBridgeModule::StartupModule()
{
#if TGMB_WITH_DESIGNER_NODES
	const FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("TextureGraphMaterialBridge"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/TextureGraphMaterialBridge"), PluginShaderDir);
#endif
}

void FTextureGraphMaterialBridgeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTextureGraphMaterialBridgeModule, TextureGraphMaterialBridge)
