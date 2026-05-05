#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"

class FObjectPostSaveContext;
class UMaterial;
class UPackage;
class UMaterialExpressionTextureGraphOutput;
class UMaterialExpressionTextureGraphSample;
class UTextureGraphBase;

class FTextureGraphMaterialBridgeSaveService
{
public:
	void Startup();
	void Shutdown();

private:
	void HandlePackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);
	TArray<FSoftObjectPath> FindReferencingMaterialPaths(const UTextureGraphBase* TextureGraph) const;
	static bool MaterialReferencesTextureGraph(const UMaterial* Material, const UTextureGraphBase* TextureGraph);
	static void RecompileMaterials(const TArray<FSoftObjectPath>& MaterialPaths, const UTextureGraphBase* TextureGraph);
	static void RefreshExportedTextureResources(const UTextureGraphBase* TextureGraph);
	static void RefreshReferencedTextureGraphExpressions(UMaterial* Material, const UTextureGraphBase* TextureGraph);
	static void ForceRefreshMaterialEditorPreviews(UMaterial* Material);
	static void RefreshTextureGraphOutputExpression(UMaterialExpressionTextureGraphOutput* TextureGraphExpression);
	static void RefreshTextureGraphSampleExpression(UMaterialExpressionTextureGraphSample* TextureGraphExpression);

	FDelegateHandle PackageSavedHandle;
};
