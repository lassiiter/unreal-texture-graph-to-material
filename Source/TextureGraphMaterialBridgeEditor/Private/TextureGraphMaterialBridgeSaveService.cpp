#include "TextureGraphMaterialBridgeSaveService.h"

#include "TextureGraphMaterialBridgeEditorExportUtils.h"
#include "TextureGraphMaterialBridgeEditorModule.h"
#include "TextureGraphMaterialBridgeExpressionUtils.h"
#include "TextureGraphMaterialBridgeMaterialInstanceBindingService.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Async/Async.h"
#include "Engine/Texture.h"
#include "Editor.h"
#include "IMaterialEditor.h"
#include "MaterialEditorUtilities.h"
#include "MaterialEditingLibrary.h"
#include "MaterialExpressionTextureGraphOutput.h"
#include "MaterialExpressionTextureGraphSample.h"
#include "MaterialGraph/MaterialGraph.h"
#include "Materials/Material.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TG_HelperFunctions.h"
#include "TextureGraph.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

DEFINE_LOG_CATEGORY_STATIC(LogTextureGraphMaterialBridgeSaveService, Log, All);

void FTextureGraphMaterialBridgeSaveService::Startup()
{
	PackageSavedHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FTextureGraphMaterialBridgeSaveService::HandlePackageSaved);
}

void FTextureGraphMaterialBridgeSaveService::Shutdown()
{
	if (PackageSavedHandle.IsValid())
	{
		UPackage::PackageSavedWithContextEvent.Remove(PackageSavedHandle);
		PackageSavedHandle.Reset();
	}
}

void FTextureGraphMaterialBridgeSaveService::HandlePackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	if (!Package || ObjectSaveContext.IsProceduralSave() || ObjectSaveContext.IsFromAutoSave())
	{
		return;
	}

	UTextureGraphBase* SavedTextureGraph = Cast<UTextureGraphBase>(Package->FindAssetInPackage());
	if (!SavedTextureGraph)
	{
		return;
	}

	UE::TextureGraphMaterialBridgeEditor::EnsureAllLiveTextureGraphTargetsInitialized();

	TArray<FSoftObjectPath> ReferencingMaterialPaths = FindReferencingMaterialPaths(SavedTextureGraph);
	TArray<FSoftObjectPath> BoundMaterialInstancePaths;
	if (FTextureGraphMaterialBridgeEditorModule* EditorModule = FModuleManager::GetModulePtr<FTextureGraphMaterialBridgeEditorModule>("TextureGraphMaterialBridgeEditor"))
	{
		BoundMaterialInstancePaths = EditorModule->GetMaterialInstanceBindingService().FindBoundMaterialInstancePaths(SavedTextureGraph);
	}

	if (ReferencingMaterialPaths.IsEmpty() && BoundMaterialInstancePaths.IsEmpty())
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Verbose, TEXT("TextureGraphMaterialBridge skipped refresh for '%s' because no referencing materials or bound material instances were found."), *SavedTextureGraph->GetPathName());
		return;
	}

	if (ReferencingMaterialPaths.IsEmpty())
	{
		RefreshMaterialInstanceBindings(SavedTextureGraph, BoundMaterialInstancePaths);
		return;
	}

	const UE::TextureGraphMaterialBridgeEditor::FResolvedTextureGraphExportSource ExportSource =
		UE::TextureGraphMaterialBridgeEditor::ResolveExportTextureGraph(SavedTextureGraph);
	if (!ExportSource.TextureGraph)
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Warning, TEXT("TextureGraphMaterialBridge could not resolve an export graph for '%s'."), *SavedTextureGraph->GetPathName());
		RefreshMaterialInstanceBindings(SavedTextureGraph, BoundMaterialInstancePaths);
		return;
	}

	if (ExportSource.bExportSourceDirectly)
	{
		FExportSettings ExportSettings;
		FString SavedTextureGraphPath = SavedTextureGraph->GetPathName();

		UE_LOG(
			LogTextureGraphMaterialBridgeSaveService,
			Log,
			TEXT("TextureGraphMaterialBridge exporting '%s' directly using %s '%s'."),
			*SavedTextureGraphPath,
			ExportSource.SourceDescription,
			*ExportSource.TextureGraph->GetPathName());

		FTG_HelperFunctions::ExportAsync(ExportSource.TextureGraph, TEXT(""), TEXT(""), ExportSettings, false, true, false, true)
			.then(
				[MaterialPaths = MoveTemp(ReferencingMaterialPaths),
				 BoundMaterialInstancePaths = MoveTemp(BoundMaterialInstancePaths),
				 SavedTextureGraphPath,
				 SavedTextureGraph = TWeakObjectPtr<UTextureGraphBase>(SavedTextureGraph)](int32 NumExports) mutable
		{
			AsyncTask(
				ENamedThreads::GameThread,
				[MaterialPaths = MoveTemp(MaterialPaths),
				 BoundMaterialInstancePaths = MoveTemp(BoundMaterialInstancePaths),
				 SavedTextureGraphPath = MoveTemp(SavedTextureGraphPath),
				 SavedTextureGraph = MoveTemp(SavedTextureGraph),
				 NumExports]() mutable
			{
				if (NumExports > 0)
				{
					UE_LOG(
						LogTextureGraphMaterialBridgeSaveService,
						Verbose,
						TEXT("TextureGraphMaterialBridge exported %d texture(s) for '%s'; recompiling %d referencing material(s)."),
						NumExports,
						*SavedTextureGraphPath,
						MaterialPaths.Num());

					RecompileMaterials(MaterialPaths, SavedTextureGraph.Get());
				}
				else
				{
					UE_LOG(
						LogTextureGraphMaterialBridgeSaveService,
						Warning,
						TEXT("TextureGraphMaterialBridge skipped material recompilation for '%s' because the save-triggered export produced no textures."),
						*SavedTextureGraphPath);
				}

				RefreshMaterialInstanceBindings(SavedTextureGraph.Get(), BoundMaterialInstancePaths);
			});

			return NumExports;
		});

		return;
	}

	UTextureGraphBase* PreparedExportTextureGraph = UE::TextureGraphMaterialBridgeEditor::CreatePreparedExportTextureGraph(ExportSource.TextureGraph);
	UE::TextureGraphMaterialBridgeEditor::CleanupExportTextureGraph(ExportSource.TextureGraph, ExportSource.bRequiresCleanup);
	if (!PreparedExportTextureGraph)
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Warning, TEXT("TextureGraphMaterialBridge could not prepare an export graph for '%s'."), *SavedTextureGraph->GetPathName());
		RefreshMaterialInstanceBindings(SavedTextureGraph, BoundMaterialInstancePaths);
		return;
	}

	FExportSettings ExportSettings;
	TStrongObjectPtr<UTextureGraphBase> ExportTextureGraph(PreparedExportTextureGraph);

	UE_LOG(
		LogTextureGraphMaterialBridgeSaveService,
		Verbose,
		TEXT("TextureGraphMaterialBridge exporting '%s' using %s '%s'."),
		*SavedTextureGraph->GetPathName(),
		ExportSource.SourceDescription,
		*ExportTextureGraph->GetPathName());

	UE::TextureGraphMaterialBridgeEditor::ExportPreparedTextureGraphAsync(ExportTextureGraph.Get(), ExportSettings)
		.then(
			[MaterialPaths = MoveTemp(ReferencingMaterialPaths),
			 BoundMaterialInstancePaths = MoveTemp(BoundMaterialInstancePaths),
			 ExportTextureGraph = MoveTemp(ExportTextureGraph),
			 SavedTextureGraphPath = SavedTextureGraph->GetPathName(),
			 SavedTextureGraph = TWeakObjectPtr<UTextureGraphBase>(SavedTextureGraph),
			 bRequiresCleanup = true](int32 NumExports) mutable
		{
			AsyncTask(
				ENamedThreads::GameThread,
				[MaterialPaths = MoveTemp(MaterialPaths),
				 BoundMaterialInstancePaths = MoveTemp(BoundMaterialInstancePaths),
				 ExportTextureGraph = MoveTemp(ExportTextureGraph),
				 SavedTextureGraphPath = MoveTemp(SavedTextureGraphPath),
				 SavedTextureGraph = MoveTemp(SavedTextureGraph),
				 bRequiresCleanup,
				 NumExports]() mutable
			{
				UE::TextureGraphMaterialBridgeEditor::CleanupExportTextureGraph(ExportTextureGraph.Get(), bRequiresCleanup);

				if (NumExports > 0)
				{
					UE_LOG(
						LogTextureGraphMaterialBridgeSaveService,
						Verbose,
						TEXT("TextureGraphMaterialBridge exported %d texture(s) for '%s'; recompiling %d referencing material(s)."),
						NumExports,
						*SavedTextureGraphPath,
						MaterialPaths.Num());

					RecompileMaterials(MaterialPaths, SavedTextureGraph.Get());
				}
				else
				{
					UE_LOG(
						LogTextureGraphMaterialBridgeSaveService,
						Warning,
						TEXT("TextureGraphMaterialBridge skipped material recompilation for '%s' because the save-triggered export produced no textures."),
						*SavedTextureGraphPath);
				}

				RefreshMaterialInstanceBindings(SavedTextureGraph.Get(), BoundMaterialInstancePaths);
			});

			return NumExports;
		});
}

TArray<FSoftObjectPath> FTextureGraphMaterialBridgeSaveService::FindReferencingMaterialPaths(const UTextureGraphBase* TextureGraph) const
{
	TArray<FSoftObjectPath> MaterialPaths;
	if (!TextureGraph)
	{
		return MaterialPaths;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FName> ReferencerPackages;
	AssetRegistry.GetReferencers(
		TextureGraph->GetOutermost()->GetFName(),
		ReferencerPackages,
		UE::AssetRegistry::EDependencyCategory::Package,
		UE::AssetRegistry::EDependencyQuery::Hard);

	for (const FName ReferencerPackage : ReferencerPackages)
	{
		TArray<FAssetData> AssetsInPackage;
		if (!AssetRegistry.GetAssetsByPackageName(ReferencerPackage, AssetsInPackage))
		{
			continue;
		}

		for (const FAssetData& AssetData : AssetsInPackage)
		{
			if (!AssetData.IsInstanceOf(UMaterial::StaticClass()))
			{
				continue;
			}

			UMaterial* Material = Cast<UMaterial>(AssetData.GetAsset());
			if (!Material || !MaterialReferencesTextureGraph(Material, TextureGraph))
			{
				continue;
			}

			MaterialPaths.AddUnique(AssetData.ToSoftObjectPath());
		}
	}

	return MaterialPaths;
}

bool FTextureGraphMaterialBridgeSaveService::MaterialReferencesTextureGraph(const UMaterial* Material, const UTextureGraphBase* TextureGraph)
{
	if (!Material || !TextureGraph)
	{
		return false;
	}

	for (UMaterialExpression* Expression : Material->GetExpressions())
	{
		if (const UMaterialExpressionTextureGraphOutput* TextureGraphExpression = Cast<UMaterialExpressionTextureGraphOutput>(Expression))
		{
			if (TextureGraphExpression->ReferencesTextureGraph(TextureGraph))
			{
				return true;
			}
		}

		if (const UMaterialExpressionTextureGraphSample* TextureGraphSampleExpression = Cast<UMaterialExpressionTextureGraphSample>(Expression))
		{
			if (TextureGraphSampleExpression->ReferencesTextureGraph(TextureGraph))
			{
				return true;
			}
		}
	}

	return false;
}

void FTextureGraphMaterialBridgeSaveService::RecompileMaterials(const TArray<FSoftObjectPath>& MaterialPaths, const UTextureGraphBase* TextureGraph)
{
	RefreshExportedTextureResources(TextureGraph);

	for (const FSoftObjectPath& MaterialPath : MaterialPaths)
	{
		UMaterial* Material = Cast<UMaterial>(MaterialPath.TryLoad());
		if (!Material)
		{
			continue;
		}

		RefreshReferencedTextureGraphExpressions(Material, TextureGraph);
		UMaterialEditingLibrary::RecompileMaterial(Material);
		ForceRefreshMaterialEditorPreviews(Material);
	}
}

void FTextureGraphMaterialBridgeSaveService::RefreshMaterialInstanceBindings(UTextureGraphBase* TextureGraph, const TArray<FSoftObjectPath>& MaterialInstancePaths)
{
	if (!TextureGraph || MaterialInstancePaths.IsEmpty())
	{
		return;
	}

	FTextureGraphMaterialBridgeEditorModule* EditorModule = FModuleManager::GetModulePtr<FTextureGraphMaterialBridgeEditorModule>("TextureGraphMaterialBridgeEditor");
	if (!EditorModule)
	{
		return;
	}

	EditorModule->GetMaterialInstanceBindingService().RefreshBindingsForTextureGraph(TextureGraph, MaterialInstancePaths);
}

void FTextureGraphMaterialBridgeSaveService::RefreshExportedTextureResources(const UTextureGraphBase* TextureGraph)
{
	if (!TextureGraph)
	{
		return;
	}

	TArray<FName> OutputNames;
	UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraph, OutputNames);

	TSet<UTexture*> RefreshedTextures;
	for (const FName OutputName : OutputNames)
	{
		UTexture* Texture = UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraph, OutputName, nullptr);
		if (!Texture)
		{
			continue;
		}

		if (RefreshedTextures.Contains(Texture))
		{
			continue;
		}

		Texture->UpdateResource();
		RefreshedTextures.Add(Texture);
	}
}

void FTextureGraphMaterialBridgeSaveService::RefreshReferencedTextureGraphExpressions(UMaterial* Material, const UTextureGraphBase* TextureGraph)
{
	if (!Material || !TextureGraph)
	{
		return;
	}

	for (UMaterialExpression* Expression : Material->GetExpressions())
	{
		if (UMaterialExpressionTextureGraphOutput* TextureGraphExpression = Cast<UMaterialExpressionTextureGraphOutput>(Expression))
		{
			if (TextureGraphExpression->ReferencesTextureGraph(TextureGraph))
			{
				RefreshTextureGraphOutputExpression(TextureGraphExpression);
			}
		}

		if (UMaterialExpressionTextureGraphSample* TextureGraphSampleExpression = Cast<UMaterialExpressionTextureGraphSample>(Expression))
		{
			if (TextureGraphSampleExpression->ReferencesTextureGraph(TextureGraph))
			{
				RefreshTextureGraphSampleExpression(TextureGraphSampleExpression);
			}
		}
	}
}

void FTextureGraphMaterialBridgeSaveService::ForceRefreshMaterialEditorPreviews(UMaterial* Material)
{
	if (!Material)
	{
		return;
	}

	if (UMaterialGraph* MaterialGraph = Material->MaterialGraph.Get())
	{
		FMaterialEditorUtilities::ForceRefreshExpressionPreviews(MaterialGraph);
		return;
	}

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr;
	if (!AssetEditorSubsystem)
	{
		return;
	}

	TArray<IAssetEditorInstance*> EditorInstances = AssetEditorSubsystem->FindEditorsForAsset(Material);
	if (EditorInstances.IsEmpty())
	{
		EditorInstances = AssetEditorSubsystem->FindEditorsForAssetAndSubObjects(Material);
	}

	if (EditorInstances.IsEmpty())
	{
		return;
	}

	for (IAssetEditorInstance* EditorInstance : EditorInstances)
	{
		if (!EditorInstance)
		{
			continue;
		}

		if (EditorInstance->GetEditorName() != FName(TEXT("MaterialEditor")))
		{
			continue;
		}

		IMaterialEditor* MaterialEditor = static_cast<IMaterialEditor*>(EditorInstance);
		if (!MaterialEditor)
		{
			continue;
		}

		MaterialEditor->UpdateDetailView();
		MaterialEditor->ForceRefreshExpressionPreviews();
		MaterialEditor->RefreshStatsMaterials();
		return;
	}
}

void FTextureGraphMaterialBridgeSaveService::RefreshTextureGraphOutputExpression(UMaterialExpressionTextureGraphOutput* TextureGraphExpression)
{
	if (!TextureGraphExpression)
	{
		return;
	}

	TextureGraphExpression->UpdateOutputs();

	if (UEdGraphNode* GraphNode = TextureGraphExpression->GetEditorGraphNode())
	{
		GraphNode->ReconstructNode();
	}
}

void FTextureGraphMaterialBridgeSaveService::RefreshTextureGraphSampleExpression(UMaterialExpressionTextureGraphSample* TextureGraphExpression)
{
	if (!TextureGraphExpression)
	{
		return;
	}

	TextureGraphExpression->RefreshResolvedTextureGraphOutput();
}
