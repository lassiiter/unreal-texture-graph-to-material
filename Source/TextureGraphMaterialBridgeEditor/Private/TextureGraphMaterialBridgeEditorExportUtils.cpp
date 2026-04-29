#include "TextureGraphMaterialBridgeEditorExportUtils.h"

#include "Async/Async.h"
#include "Export/TextureExporter.h"
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
	namespace
	{
		bool ReferencesSavedTextureGraph(const ITG_Editor* TextureGraphEditor, const UTextureGraph* SavedTextureGraph)
		{
			if (!TextureGraphEditor || !SavedTextureGraph)
			{
				return false;
			}

			const TArray<UObject*>* EditedObjects = TextureGraphEditor->GetObjectsCurrentlyBeingEdited();
			if (!EditedObjects)
			{
				return false;
			}

			const FString SavedTextureGraphPath = SavedTextureGraph->GetPathName();
			const UPackage* SavedPackage = SavedTextureGraph->GetOutermost();
			for (const UObject* EditedObject : *EditedObjects)
			{
				if (!EditedObject)
				{
					continue;
				}

				if (EditedObject == SavedTextureGraph || EditedObject->GetPathName() == SavedTextureGraphPath || EditedObject->GetOutermost() == SavedPackage)
				{
					return true;
				}
			}

			return false;
		}

		UTextureGraphBase* ResolveOpenEditorTextureGraph(IAssetEditorInstance* EditorInstance, const UTextureGraph* SavedTextureGraph)
		{
			if (!EditorInstance || EditorInstance->GetEditorName() != FName(TEXT("TG_Editor")))
			{
				return nullptr;
			}

			ITG_Editor* TextureGraphEditor = static_cast<ITG_Editor*>(EditorInstance);
			if (!ReferencesSavedTextureGraph(TextureGraphEditor, SavedTextureGraph))
			{
				return nullptr;
			}

			return TextureGraphEditor ? Cast<UTextureGraphBase>(TextureGraphEditor->GetTextureGraphInterface()) : nullptr;
		}
	}

	FResolvedTextureGraphExportSource ResolveExportTextureGraph(UTextureGraph* SavedTextureGraph)
	{
		if (SavedTextureGraph && GEditor)
		{
			if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				if (UTextureGraphBase* EditorTextureGraph = ResolveOpenEditorTextureGraph(AssetEditorSubsystem->FindEditorForAsset(SavedTextureGraph, false), SavedTextureGraph))
				{
					return { EditorTextureGraph, false, true, TEXT("open editor graph") };
				}

				for (IAssetEditorInstance* EditorInstance : AssetEditorSubsystem->GetAllOpenEditors())
				{
					if (UTextureGraphBase* EditorTextureGraph = ResolveOpenEditorTextureGraph(EditorInstance, SavedTextureGraph))
					{
						return { EditorTextureGraph, false, true, TEXT("open editor graph") };
					}
				}
			}
		}

		if (SavedTextureGraph)
		{
			return { SavedTextureGraph, false, false, TEXT("saved asset graph") };
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
