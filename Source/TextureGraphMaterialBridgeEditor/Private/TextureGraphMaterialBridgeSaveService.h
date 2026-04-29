#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"

class FObjectPostSaveContext;
class UMaterial;
class UPackage;
class UMaterialExpressionTextureGraphOutput;
class UTextureGraph;

class FTextureGraphMaterialBridgeSaveService
{
public:
	void Startup();
	void Shutdown();

private:
	void HandlePackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);
	TArray<FSoftObjectPath> FindReferencingMaterialPaths(const UTextureGraph* TextureGraph) const;
	static bool MaterialReferencesTextureGraph(const UMaterial* Material, const UTextureGraph* TextureGraph);
	static void RecompileMaterials(const TArray<FSoftObjectPath>& MaterialPaths, const UTextureGraph* TextureGraph);
	static void RefreshReferencedTextureGraphOutputs(UMaterial* Material, const UTextureGraph* TextureGraph);
	static void RefreshTextureGraphOutputExpression(UMaterialExpressionTextureGraphOutput* TextureGraphExpression);

	FDelegateHandle PackageSavedHandle;
};
