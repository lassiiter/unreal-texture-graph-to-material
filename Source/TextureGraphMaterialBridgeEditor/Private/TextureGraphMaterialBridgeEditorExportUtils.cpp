#include "TextureGraphMaterialBridgeEditorExportUtils.h"

#include "TextureGraphMaterialBridgeExpressionUtils.h"
#include "Async/Async.h"
#include "Export/TextureExporter.h"
#include "Expressions/Output/TG_Expression_Output.h"
#include "ITG_Editor.h"
#include "Model/Mix/MixSettings.h"
#include "TG_Graph.h"
#include "TG_HelperFunctions.h"
#include "TG_Node.h"
#include "TG_Pin.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TextureGraph.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"

#include "Editor.h"

namespace UE::TextureGraphMaterialBridgeEditor
{
	namespace
	{
		bool ReferencesSavedTextureGraph(const ITG_Editor* TextureGraphEditor, const UTextureGraphBase* SavedTextureGraph)
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

		UTextureGraphBase* ResolveOpenEditorTextureGraph(IAssetEditorInstance* EditorInstance, const UTextureGraphBase* SavedTextureGraph)
		{
			if (!EditorInstance)
			{
				return nullptr;
			}

			const FName EditorName = EditorInstance->GetEditorName();
			if (EditorName != FName(TEXT("TG_Editor")) && EditorName != FName(TEXT("TG_InstanceEditor")))
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

		void EnsureTextureGraphInstanceInitialized(UTextureGraphBase* TextureGraph)
		{
			if (UTextureGraphInstance* TextureGraphInstance = Cast<UTextureGraphInstance>(TextureGraph))
			{
				if (!static_cast<const UTextureGraphInstance*>(TextureGraphInstance)->Graph())
				{
					TextureGraphInstance->Initialize();
				}
			}
		}

		UTG_Expression_Output* FindMutableTextureGraphOutputExpression(UTextureGraphBase* TextureGraph, FName InOutputName)
		{
			if (!TextureGraph || InOutputName.IsNone())
			{
				return nullptr;
			}

			UTG_Graph* Graph = TextureGraph->Graph();
			if (!Graph)
			{
				return nullptr;
			}

			UTG_Expression_Output* FoundOutput = nullptr;
			Graph->ForEachNodes([InOutputName, &FoundOutput](const UTG_Node* Node, uint32)
			{
				if (FoundOutput || !Node)
				{
					return;
				}

				if (UTG_Expression_Output* OutputExpression = Cast<UTG_Expression_Output>(Node->GetExpression()))
				{
					const FName CanonicalOutputName = UE::TextureGraphMaterialBridge::GetTextureGraphOutputName(*OutputExpression);
					if (CanonicalOutputName == InOutputName || OutputExpression->GetTitleName() == InOutputName || OutputExpression->OutputSettings.OutputName == InOutputName)
					{
						FoundOutput = OutputExpression;
					}
				}
			});

			return FoundOutput;
		}

		void ApplyOutputSettingsToExpression(UTG_Expression_Output* OutputExpression, const FTG_OutputSettings& OutputSettings)
		{
			if (!OutputExpression)
			{
				return;
			}

			OutputExpression->OutputSettings = OutputSettings;

			if (UTG_Node* Node = OutputExpression->GetParentNode())
			{
				if (UTG_Pin* SettingsPin = Node->GetPin(GET_MEMBER_NAME_CHECKED(UTG_Expression_Output, OutputSettings)))
				{
					SettingsPin->FromString(OutputExpression->OutputSettings.ToString());
				}
			}
		}

		TArray<TPair<FName, FTG_OutputSettings>> ApplyEffectiveOutputSettingsForExport(UTextureGraphBase* TextureGraph)
		{
			TArray<TPair<FName, FTG_OutputSettings>> OriginalOutputSettings;

			if (!TextureGraph)
			{
				return OriginalOutputSettings;
			}

			const UTG_Graph* SourceGraph = TextureGraph->Graph();
			if (!SourceGraph)
			{
				return OriginalOutputSettings;
			}

			SourceGraph->ForEachNodes([TextureGraph, &OriginalOutputSettings](const UTG_Node* Node, uint32)
			{
				if (!Node)
				{
					return;
				}

				const UTG_Expression_Output* SourceOutputExpression = Cast<UTG_Expression_Output>(Node->GetExpression());
				if (!SourceOutputExpression)
				{
					return;
				}

				const FName OutputName = UE::TextureGraphMaterialBridge::GetTextureGraphOutputName(*SourceOutputExpression);
				UTG_Expression_Output* MutableOutputExpression = FindMutableTextureGraphOutputExpression(TextureGraph, OutputName);
				if (!MutableOutputExpression)
				{
					return;
				}

				const FTG_OutputSettings EffectiveOutputSettings =
					UE::TextureGraphMaterialBridge::GetEffectiveTextureGraphOutputSettings(TextureGraph, *SourceOutputExpression);
				if (MutableOutputExpression->OutputSettings == EffectiveOutputSettings)
				{
					return;
				}

				OriginalOutputSettings.Emplace(OutputName, MutableOutputExpression->OutputSettings);
				ApplyOutputSettingsToExpression(MutableOutputExpression, EffectiveOutputSettings);
			});

			return OriginalOutputSettings;
		}

		void RestoreOutputSettingsAfterExport(UTextureGraphBase* TextureGraph, const TArray<TPair<FName, FTG_OutputSettings>>& OriginalOutputSettings)
		{
			for (const TPair<FName, FTG_OutputSettings>& OriginalOutputSetting : OriginalOutputSettings)
			{
				if (UTG_Expression_Output* OutputExpression = FindMutableTextureGraphOutputExpression(TextureGraph, OriginalOutputSetting.Key))
				{
					ApplyOutputSettingsToExpression(OutputExpression, OriginalOutputSetting.Value);
				}
			}
		}

		void RestorePackageDirtyState(UTextureGraphBase* TextureGraph, bool bWasPackageDirty)
		{
			if (TextureGraph && TextureGraph->GetOutermost())
			{
				TextureGraph->GetOutermost()->SetDirtyFlag(bWasPackageDirty);
			}
		}
	}

	FResolvedTextureGraphExportSource ResolveExportTextureGraph(UTextureGraphBase* SavedTextureGraph)
	{
		if (SavedTextureGraph && GEditor)
		{
			if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				if (UTextureGraphBase* EditorTextureGraph = ResolveOpenEditorTextureGraph(AssetEditorSubsystem->FindEditorForAsset(SavedTextureGraph, false), SavedTextureGraph))
				{
					return { EditorTextureGraph, TEXT("open editor graph") };
				}

				for (IAssetEditorInstance* EditorInstance : AssetEditorSubsystem->GetAllOpenEditors())
				{
					if (UTextureGraphBase* EditorTextureGraph = ResolveOpenEditorTextureGraph(EditorInstance, SavedTextureGraph))
					{
						return { EditorTextureGraph, TEXT("open editor graph") };
					}
				}
			}
		}

		if (SavedTextureGraph)
		{
			return { SavedTextureGraph, TEXT("saved asset graph") };
		}

		return {};
	}

	AsyncInt ExportTextureGraphDirectAsync(UTextureGraphBase* TextureGraph, FExportSettings& ExportSettings)
	{
		if (!TextureGraph)
		{
			return cti::make_ready_continuable<int32>(0);
		}

		EnsureTextureGraphTargetsInitialized(TextureGraph);

		TSharedRef<FExportSettings> SessionSettings = MakeShared<FExportSettings>(ExportSettings);
		const bool bWasPackageDirty = TextureGraph->GetOutermost() ? TextureGraph->GetOutermost()->IsDirty() : false;
		const TArray<TPair<FName, FTG_OutputSettings>> OriginalOutputSettings = ApplyEffectiveOutputSettingsForExport(TextureGraph);
		JobBatchPtr Batch = FTG_HelperFunctions::InitExportBatch(TextureGraph, TEXT(""), TEXT(""), *SessionSettings, false, true, false, true);
		RestoreOutputSettingsAfterExport(TextureGraph, OriginalOutputSettings);
		if (OriginalOutputSettings.Num() > 0)
		{
			RestorePackageDirtyState(TextureGraph, bWasPackageDirty);
		}

		if (!Batch)
		{
			AsyncTask(ENamedThreads::GameThread, [SessionSettings]()
			{
				SessionSettings->OnDone.ExecuteIfBound();
			});

			return cti::make_ready_continuable<int32>(0);
		}

		return FTG_HelperFunctions::RenderAsync(TextureGraph, Batch)
			.then([TextureGraph, SessionSettings](bool)
			{
				return TextureExporter::ExportAsUAsset(TextureGraph, SessionSettings, TEXT(""));
			})
			.then([TextureGraph](int32 NumExports)
			{
				if (IsValid(TextureGraph))
				{
					TextureGraph->InvalidateAll();
				}

				return NumExports;
			});
	}

	void EnsureTextureGraphTargetsInitialized(UTextureGraphBase* TextureGraph)
	{
		if (!TextureGraph || TextureGraph->HasAnyFlags(RF_ClassDefaultObject))
		{
			return;
		}

		EnsureTextureGraphInstanceInitialized(TextureGraph);

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
