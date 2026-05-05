#include "TextureGraphMaterialBridgeModule.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FTextureGraphMaterialBridgeModule"

DEFINE_LOG_CATEGORY(LogTextureGraphMaterialBridge);

void FTextureGraphMaterialBridgeModule::StartupModule()
{
	const FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("TextureGraphMaterialBridge"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/TextureGraphMaterialBridge"), PluginShaderDir);
}

void FTextureGraphMaterialBridgeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTextureGraphMaterialBridgeModule, TextureGraphMaterialBridge)
