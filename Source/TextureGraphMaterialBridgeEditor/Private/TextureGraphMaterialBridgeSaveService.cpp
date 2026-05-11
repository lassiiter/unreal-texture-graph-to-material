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
#include "Export/TextureExporter.h"
#include "IMaterialEditor.h"
#include "MaterialEditorUtilities.h"
#include "MaterialEditingLibrary.h"
#include "MaterialExpressionTextureGraphOutput.h"
#include "MaterialExpressionTextureGraphSample.h"
#include "MaterialGraph/MaterialGraph.h"
#include "Materials/Material.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TextureGraph.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/Package.h"

DEFINE_LOG_CATEGORY_STATIC(LogTextureGraphMaterialBridgeSaveService, Log, All);

void FTextureGraphMaterialBridgeSaveService::Startup()
{
	PackageSavedHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FTextureGraphMaterialBridgeSaveService::HandlePackageSaved);
	UE_LOG(LogTextureGraphMaterialBridgeSaveService, Verbose, TEXT("TextureGraphMaterialBridge save service registered PackageSavedWithContext handler."));
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
	if (!Package)
	{
		return;
	}

	UTextureGraphBase* SavedTextureGraph = Cast<UTextureGraphBase>(Package->FindAssetInPackage());
	if (!SavedTextureGraph)
	{
		return;
	}

	UE_LOG(
		LogTextureGraphMaterialBridgeSaveService,
		Verbose,
		TEXT("Texture Graph package saved. Graph='%s' Package='%s' File='%s' Procedural=%s AutoSave=%s"),
		*SavedTextureGraph->GetPathName(),
		*Package->GetName(),
		*PackageFilename,
		ObjectSaveContext.IsProceduralSave() ? TEXT("true") : TEXT("false"),
		ObjectSaveContext.IsFromAutoSave() ? TEXT("true") : TEXT("false"));

	if (ObjectSaveContext.IsProceduralSave() || ObjectSaveContext.IsFromAutoSave())
	{
		UE_LOG(
			LogTextureGraphMaterialBridgeSaveService,
			Verbose,
			TEXT("Ignoring Texture Graph save for '%s' because it is procedural or autosave."),
			*SavedTextureGraph->GetPathName());
		return;
	}

	UE::TextureGraphMaterialBridgeEditor::EnsureAllLiveTextureGraphTargetsInitialized();

	TArray<FSoftObjectPath> ReferencingMaterialPaths = FindReferencingMaterialPaths(SavedTextureGraph);
	TArray<FSoftObjectPath> BoundMaterialInstancePaths;
	if (FTextureGraphMaterialBridgeEditorModule* EditorModule = FModuleManager::GetModulePtr<FTextureGraphMaterialBridgeEditorModule>("TextureGraphMaterialBridgeEditor"))
	{
		BoundMaterialInstancePaths = EditorModule->GetMaterialInstanceBindingService().FindBoundMaterialInstancePaths(SavedTextureGraph);
	}
	else
	{
		UE_LOG(
			LogTextureGraphMaterialBridgeSaveService,
			Warning,
			TEXT("Could not inspect bound Material Instances because TextureGraphMaterialBridgeEditor module was not available."));
	}

	UE_LOG(
		LogTextureGraphMaterialBridgeSaveService,
		Log,
		TEXT("TextureGraphMaterialBridge save candidates for '%s': ReferencingMaterials=%d, IndexedMaterialInstances=%d."),
		*SavedTextureGraph->GetPathName(),
		ReferencingMaterialPaths.Num(),
		BoundMaterialInstancePaths.Num());

	if (ReferencingMaterialPaths.IsEmpty() && BoundMaterialInstancePaths.IsEmpty())
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Verbose, TEXT("TextureGraphMaterialBridge skipped export/refresh for '%s' because no referencing materials or indexed Material Instances were found."), *SavedTextureGraph->GetPathName());
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

	FExportSettings ExportSettings;

	UE_LOG(
		LogTextureGraphMaterialBridgeSaveService,
		Log,
		TEXT("TextureGraphMaterialBridge exporting '%s' directly using %s '%s'. ReferencingMaterials=%d IndexedMaterialInstances=%d."),
		*SavedTextureGraph->GetPathName(),
		ExportSource.SourceDescription,
		*ExportSource.TextureGraph->GetPathName(),
		ReferencingMaterialPaths.Num(),
		BoundMaterialInstancePaths.Num());

	UE::TextureGraphMaterialBridgeEditor::ExportTextureGraphDirectAsync(ExportSource.TextureGraph, ExportSettings)
		.then(
			[MaterialPaths = MoveTemp(ReferencingMaterialPaths),
			 BoundMaterialInstancePaths = MoveTemp(BoundMaterialInstancePaths),
			 SavedTextureGraphPath = SavedTextureGraph->GetPathName(),
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
						Log,
						TEXT("TextureGraphMaterialBridge exported %d texture(s) for '%s'; recompiling %d referencing material(s), refreshing %d indexed Material Instance(s)."),
						NumExports,
						*SavedTextureGraphPath,
						MaterialPaths.Num(),
						BoundMaterialInstancePaths.Num());

					RecompileMaterials(MaterialPaths, SavedTextureGraph.Get());
				}
				else
				{
					UE_LOG(
						LogTextureGraphMaterialBridgeSaveService,
						Warning,
						TEXT("TextureGraphMaterialBridge save-triggered export produced no textures for '%s'. ReferencingMaterials=%d IndexedMaterialInstances=%d."),
						*SavedTextureGraphPath,
						MaterialPaths.Num(),
						BoundMaterialInstancePaths.Num());
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
