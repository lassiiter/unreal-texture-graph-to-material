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
		bool bRequiresCleanup = false;
		bool bExportSourceDirectly = false;
		const TCHAR* SourceDescription = TEXT("unresolved");
	};

	FResolvedTextureGraphExportSource ResolveExportTextureGraph(UTextureGraphBase* SavedTextureGraph);
	UTextureGraphBase* CreatePreparedExportTextureGraph(UTextureGraphBase* SourceTextureGraph);
	UTextureGraphBase* CreatePreparedExportTextureGraphInstance(UTextureGraphBase* SourceTextureGraph);
	AsyncInt ExportPreparedTextureGraphAsync(UTextureGraphBase* PreparedTextureGraph, FExportSettings& ExportSettings);
	void CleanupExportTextureGraph(UTextureGraphBase* TextureGraph, bool bRequiresCleanup);
	void EnsureTextureGraphTargetsInitialized(UTextureGraphBase* TextureGraph);
	void EnsureAllLiveTextureGraphTargetsInitialized();
}
