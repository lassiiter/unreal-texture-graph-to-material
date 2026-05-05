#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpression.h"
#include "TextureGraph.h"
#include "MaterialExpressionTextureGraphOutput.generated.h"

class UTexture;
class UTexture2DArray;
class UTextureCube;
class UTextureCubeArray;
class UVolumeTexture;
class UEdGraphNode;
class UTG_Expression_Output;

enum class ETextureGraphOutputNodeStatus : uint8
{
	NoGraphAssigned,
	NoOutputsFound,
	Ready,
	Partial,
	MissingExport
};

UCLASS(collapsecategories, hidecategories = Object, DisplayName = "Texture Graph Output")
class TEXTUREGRAPHMATERIALBRIDGE_API UMaterialExpressionTextureGraphOutput : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UMaterialExpressionTextureGraphOutput(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	TObjectPtr<UTextureGraphBase> TextureGraphAsset;

	bool ReferencesTextureGraph(const UTextureGraphBase* InTextureGraph) const;

	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const override;
	virtual bool IsAllowedIn(const UObject* MaterialOrFunction) const override;

#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual FText GetKeywords() const override;
	virtual EMaterialValueType GetOutputValueType(int32 OutputIndex) override;
	virtual TSharedPtr<class SGraphNodeMaterialBase> CreateCustomGraphNodeWidget() override;
	virtual FText GetCreationName() const override;
	virtual FText GetCreationDescription() const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	ETextureGraphOutputNodeStatus GetOutputNodeStatus(int32& OutReadyCount, int32& OutOutputCount) const;
	void UpdateOutputs();
	UEdGraphNode* GetEditorGraphNode() const;
#endif

private:
#if WITH_EDITOR
	static EMaterialSamplerType GetSamplerTypeForResolvedTexture(const UTexture* Texture);
	static FText GetNodeTitleText();
#endif
};
