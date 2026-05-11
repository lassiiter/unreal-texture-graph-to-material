#pragma once

#include "CoreMinimal.h"
#include "Helper/Promise.h"

class UTextureGraphBase;
class FExportSettings;

namespace UE::TextureGraphMaterialBridgeEditor
{
	struct FResolvedTextureGraphExportSource
	{
		UTextureGraphBase* TextureGraph = nullptr;
		const TCHAR* SourceDescription = TEXT("unresolved");
	};

	FResolvedTextureGraphExportSource ResolveExportTextureGraph(UTextureGraphBase* SavedTextureGraph);
	AsyncInt ExportTextureGraphDirectAsync(UTextureGraphBase* TextureGraph, FExportSettings& ExportSettings);
	void EnsureTextureGraphTargetsInitialized(UTextureGraphBase* TextureGraph);
	void EnsureAllLiveTextureGraphTargetsInitialized();
}
