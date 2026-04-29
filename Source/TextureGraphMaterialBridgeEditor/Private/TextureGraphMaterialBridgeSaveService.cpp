#include "TextureGraphMaterialBridgeSaveService.h"

#include "TextureGraphMaterialBridgeEditorExportUtils.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Async/Async.h"
#include "MaterialEditorUtilities.h"
#include "MaterialEditingLibrary.h"
#include "MaterialExpressionTextureGraphOutput.h"
#include "MaterialExpressionTextureGraphSample.h"
#include "MaterialGraph/MaterialGraph.h"
#include "Materials/Material.h"
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

	UTextureGraph* SavedTextureGraph = Cast<UTextureGraph>(Package->FindAssetInPackage());
	if (!SavedTextureGraph)
	{
		return;
	}

	UE::TextureGraphMaterialBridgeEditor::EnsureAllLiveTextureGraphTargetsInitialized();

	TArray<FSoftObjectPath> ReferencingMaterialPaths = FindReferencingMaterialPaths(SavedTextureGraph);
	if (ReferencingMaterialPaths.IsEmpty())
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Verbose, TEXT("TextureGraphMaterialBridge skipped refresh for '%s' because no referencing materials were found."), *SavedTextureGraph->GetPathName());
		return;
	}

	const UE::TextureGraphMaterialBridgeEditor::FResolvedTextureGraphExportSource ExportSource =
		UE::TextureGraphMaterialBridgeEditor::ResolveExportTextureGraph(SavedTextureGraph);
	if (!ExportSource.TextureGraph)
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Warning, TEXT("TextureGraphMaterialBridge could not resolve an export graph for '%s'."), *SavedTextureGraph->GetPathName());
		return;
	}

	UTextureGraphBase* PreparedExportTextureGraph = UE::TextureGraphMaterialBridgeEditor::CreatePreparedExportTextureGraph(ExportSource.TextureGraph);
	if (!PreparedExportTextureGraph)
	{
		UE_LOG(LogTextureGraphMaterialBridgeSaveService, Warning, TEXT("TextureGraphMaterialBridge could not prepare an export graph for '%s'."), *SavedTextureGraph->GetPathName());
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
			 ExportTextureGraph = MoveTemp(ExportTextureGraph),
			 SavedTextureGraphPath = SavedTextureGraph->GetPathName(),
			 SavedTextureGraph = TWeakObjectPtr<UTextureGraph>(SavedTextureGraph),
			 bRequiresCleanup = true](int32 NumExports) mutable
		{
			AsyncTask(
				ENamedThreads::GameThread,
				[MaterialPaths = MoveTemp(MaterialPaths),
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
			});

			return NumExports;
		});
}

TArray<FSoftObjectPath> FTextureGraphMaterialBridgeSaveService::FindReferencingMaterialPaths(const UTextureGraph* TextureGraph) const
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

bool FTextureGraphMaterialBridgeSaveService::MaterialReferencesTextureGraph(const UMaterial* Material, const UTextureGraph* TextureGraph)
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

void FTextureGraphMaterialBridgeSaveService::RecompileMaterials(const TArray<FSoftObjectPath>& MaterialPaths, const UTextureGraph* TextureGraph)
{
	for (const FSoftObjectPath& MaterialPath : MaterialPaths)
	{
		UMaterial* Material = Cast<UMaterial>(MaterialPath.TryLoad());
		if (!Material)
		{
			continue;
		}

		RefreshReferencedTextureGraphOutputs(Material, TextureGraph);
		UMaterialEditingLibrary::RecompileMaterial(Material);

		if (UMaterialGraph* MaterialGraph = Material->MaterialGraph.Get())
		{
			FMaterialEditorUtilities::ForceRefreshExpressionPreviews(MaterialGraph);
		}
	}
}

void FTextureGraphMaterialBridgeSaveService::RefreshReferencedTextureGraphOutputs(UMaterial* Material, const UTextureGraph* TextureGraph)
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
