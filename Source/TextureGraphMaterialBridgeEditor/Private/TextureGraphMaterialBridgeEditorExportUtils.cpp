#include "TextureGraphMaterialBridgeEditorExportUtils.h"

#include "ITG_Editor.h"
#include "Model/Mix/MixSettings.h"
#include "TG_HelperFunctions.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TextureGraph.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UObjectGlobals.h"

#include "Editor.h"

namespace UE::TextureGraphMaterialBridgeEditor
{
	FResolvedTextureGraphExportSource ResolveExportTextureGraph(UTextureGraph* SavedTextureGraph)
	{
		if (SavedTextureGraph)
		{
			return { SavedTextureGraph, false, TEXT("saved asset graph") };
		}

		return {};
	}

	UTextureGraphBase* CreatePreparedExportTextureGraph(UTextureGraphBase* SourceTextureGraph)
	{
		if (!SourceTextureGraph)
		{
			return nullptr;
		}

		UTextureGraphBase* PreparedTextureGraph = Cast<UTextureGraphBase>(
			StaticDuplicateObject(SourceTextureGraph, GetTransientPackage(), NAME_None, RF_Standalone));
		if (!PreparedTextureGraph)
		{
			return nullptr;
		}

		PreparedTextureGraph->Initialize();
		FTG_HelperFunctions::InitTargets(PreparedTextureGraph);
		return PreparedTextureGraph;
	}

	AsyncInt ExportPreparedTextureGraphAsync(UTextureGraphBase* PreparedTextureGraph, FExportSettings& ExportSettings)
	{
		TSharedRef<FExportSettings> SessionSettings = MakeShared<FExportSettings>(ExportSettings);
		JobBatchPtr Batch = FTG_HelperFunctions::InitExportBatch(PreparedTextureGraph, TEXT(""), TEXT(""), *SessionSettings, false, true, false, true);

		if (!Batch)
		{
			AsyncTask(ENamedThreads::GameThread, [SessionSettings]()
			{
				SessionSettings->OnDone.ExecuteIfBound();
			});

			return cti::make_ready_continuable<int32>(0);
		}

		return FTG_HelperFunctions::RenderAsync(PreparedTextureGraph, Batch)
			.then([PreparedTextureGraph, SessionSettings](bool)
			{
				return TextureExporter::ExportAsUAsset(PreparedTextureGraph, SessionSettings, TEXT(""));
			});
	}

	void CleanupExportTextureGraph(UTextureGraphBase* TextureGraph, bool bRequiresCleanup)
	{
		if (!TextureGraph || !bRequiresCleanup)
		{
			return;
		}

		if (UMixSettings* Settings = TextureGraph->GetSettings())
		{
			Settings->FreeTargets();
		}

		TextureGraph->ClearFlags(RF_Standalone);
	}

	void EnsureTextureGraphTargetsInitialized(UTextureGraphBase* TextureGraph)
	{
		if (!TextureGraph || TextureGraph->HasAnyFlags(RF_ClassDefaultObject))
		{
			return;
		}

		UMixSettings* Settings = TextureGraph->GetSettings();
		if (!Settings || Settings->NumTargets() > 0 || !TextureGraph->Graph())
		{
			return;
		}

		FTG_HelperFunctions::InitTargets(TextureGraph);
	}

	void EnsureAllLiveTextureGraphTargetsInitialized()
	{
		for (TObjectIterator<UTextureGraphBase> It; It; ++It)
		{
			EnsureTextureGraphTargetsInitialized(*It);
		}
	}
}
