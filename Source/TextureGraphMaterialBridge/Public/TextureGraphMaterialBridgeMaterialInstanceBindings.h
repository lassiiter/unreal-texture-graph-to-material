#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "Materials/MaterialParameters.h"
#include "UObject/SoftObjectPath.h"

#include "TextureGraphMaterialBridgeMaterialInstanceBindings.generated.h"

class UTextureGraphBase;

UENUM()
enum class ETextureGraphMaterialInstanceBindingStatus : uint8
{
	None,
	Pending,
	Ready,
	Warning,
	Error
};

USTRUCT()
struct TEXTUREGRAPHMATERIALBRIDGE_API FTextureGraphMaterialInstanceParameterBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	FMaterialParameterInfo ParameterInfo;

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	FSoftObjectPath TextureGraphPath;

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	FName OutputName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	FSoftObjectPath ExportedTexturePath;

	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	bool bEnabled = true;

	UPROPERTY(VisibleAnywhere, Category = "Texture Graph")
	ETextureGraphMaterialInstanceBindingStatus Status = ETextureGraphMaterialInstanceBindingStatus::None;

	UPROPERTY(VisibleAnywhere, Category = "Texture Graph")
	FString StatusMessage;

	bool MatchesParameter(const FMaterialParameterInfo& InParameterInfo) const;
	bool ReferencesTextureGraph(const UTextureGraphBase* TextureGraph) const;
};

UCLASS()
class TEXTUREGRAPHMATERIALBRIDGE_API UTextureGraphMaterialInstanceBindingsAssetUserData : public UAssetUserData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Texture Graph")
	TArray<FTextureGraphMaterialInstanceParameterBinding> Bindings;

	FTextureGraphMaterialInstanceParameterBinding* FindBinding(const FMaterialParameterInfo& ParameterInfo);
	const FTextureGraphMaterialInstanceParameterBinding* FindBinding(const FMaterialParameterInfo& ParameterInfo) const;
	FTextureGraphMaterialInstanceParameterBinding& AddOrUpdateBinding(const FTextureGraphMaterialInstanceParameterBinding& Binding);
	bool RemoveBinding(const FMaterialParameterInfo& ParameterInfo);
	bool ReferencesTextureGraph(const UTextureGraphBase* TextureGraph) const;
	bool IsEmpty() const;
};
