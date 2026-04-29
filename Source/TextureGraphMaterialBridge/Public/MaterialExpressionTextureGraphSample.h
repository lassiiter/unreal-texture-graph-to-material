#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "TextureGraph.h"
#include "MaterialExpressionTextureGraphSample.generated.h"

UCLASS(collapsecategories, hidecategories = Object, DisplayName = "Texture Graph Sample")
class TEXTUREGRAPHMATERIALBRIDGE_API UMaterialExpressionTextureGraphSample : public UMaterialExpressionTextureSample
{
	GENERATED_BODY()

public:
	UMaterialExpressionTextureGraphSample(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	TObjectPtr<UTextureGraph> TextureGraphAsset;

	UPROPERTY(EditAnywhere, Category = "Texture Graph", meta = (DisplayName = "Output", GetOptions = "GetAvailableOutputNameOptions"))
	FName TargetOutputName;

	UFUNCTION()
	TArray<FString> GetAvailableOutputNameOptions() const;

	bool ReferencesTextureGraph(const UTextureGraph* InTextureGraph) const;

	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const override;
	virtual bool IsAllowedIn(const UObject* MaterialOrFunction) const override;
	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	virtual void PostLoad() override;
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual FText GetKeywords() const override;
	virtual FText GetCreationName() const override;
	virtual FText GetCreationDescription() const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void ResetMenuCategories();

#if WITH_EDITOR
	void RefreshOutputSelection();
	void SyncResolvedTexture();
	static FText GetNodeTitleText();
#endif
};
