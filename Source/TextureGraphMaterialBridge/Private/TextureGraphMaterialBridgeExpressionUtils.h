#pragma once

#include "CoreMinimal.h"

class UTexture;
class UTextureGraph;
class UTG_Expression_Output;
struct FTG_OutputSettings;

namespace UE::TextureGraphMaterialBridge
{
	TEXTUREGRAPHMATERIALBRIDGE_API FName GetTextureGraphOutputName(const UTG_Expression_Output& OutputExpression);
	TEXTUREGRAPHMATERIALBRIDGE_API void GetTextureGraphOutputNames(const UTextureGraph* TextureGraph, TArray<FName>& OutOutputNames);
	TEXTUREGRAPHMATERIALBRIDGE_API const UTG_Expression_Output* FindTextureGraphOutputExpression(const UTextureGraph* TextureGraph, FName InOutputName, FString* OutError);
	TEXTUREGRAPHMATERIALBRIDGE_API UTexture* ResolveTextureGraphExportedTexture(const UTextureGraph* TextureGraph, FName InOutputName, FString* OutError);
	TEXTUREGRAPHMATERIALBRIDGE_API FString BuildTextureGraphExportedTextureObjectPath(const FTG_OutputSettings& OutputSettings);
	TEXTUREGRAPHMATERIALBRIDGE_API FString BuildTextureGraphMaterialUsageError(const FString& DetailMessage, const UTextureGraph* InTextureGraph, FName InOutputName);
}
