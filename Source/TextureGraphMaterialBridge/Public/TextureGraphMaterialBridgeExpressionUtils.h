#pragma once

#include "CoreMinimal.h"
#include "TG_OutputSettings.h"

class UTexture;
class UTextureGraphBase;
class UTG_Expression_Output;

namespace UE::TextureGraphMaterialBridge
{
	TEXTUREGRAPHMATERIALBRIDGE_API FName GetTextureGraphOutputName(const UTG_Expression_Output& OutputExpression);
	TEXTUREGRAPHMATERIALBRIDGE_API void GetTextureGraphOutputNames(const UTextureGraphBase* TextureGraph, TArray<FName>& OutOutputNames);
	TEXTUREGRAPHMATERIALBRIDGE_API const UTG_Expression_Output* FindTextureGraphOutputExpression(const UTextureGraphBase* TextureGraph, FName InOutputName, FString* OutError);
	TEXTUREGRAPHMATERIALBRIDGE_API FTG_OutputSettings GetEffectiveTextureGraphOutputSettings(const UTextureGraphBase* TextureGraph, const UTG_Expression_Output& OutputExpression);
	TEXTUREGRAPHMATERIALBRIDGE_API UTexture* ResolveTextureGraphExportedTexture(const UTextureGraphBase* TextureGraph, FName InOutputName, FString* OutError);
	TEXTUREGRAPHMATERIALBRIDGE_API FString BuildTextureGraphExportedTextureObjectPath(const FTG_OutputSettings& OutputSettings);
	TEXTUREGRAPHMATERIALBRIDGE_API FString BuildTextureGraphMaterialUsageError(const FString& DetailMessage, const UTextureGraphBase* InTextureGraph, FName InOutputName);
}
