#include "MaterialExpressionTextureGraphOutput.h"

#include "TextureGraphMaterialBridgeExpressionUtils.h"
#include "TextureGraphMaterialBridgeEditorHooks.h"
#include "EdGraph/EdGraphNode.h"
#include "Engine/Texture.h"
#include "Engine/Texture2DArray.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureCubeArray.h"
#include "Engine/VolumeTexture.h"
#include "Expressions/Output/TG_Expression_Output.h"
#include "Materials/Material.h"
#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionTextureBase.h"

#define LOCTEXT_NAMESPACE "MaterialExpressionTextureGraphOutput"

UMaterialExpressionTextureGraphOutput::UMaterialExpressionTextureGraphOutput(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	MenuCategories.Add(LOCTEXT("TextureGraphCategory", "Texture Graph"));

	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT("")));

	bCollapsed = true;
	bHidePreviewWindow = true;
	bShowOutputNameOnPin = true;
#endif
}

bool UMaterialExpressionTextureGraphOutput::ReferencesTextureGraph(const UTextureGraph* InTextureGraph) const
{
	return TextureGraphAsset == InTextureGraph;
}

UObject* UMaterialExpressionTextureGraphOutput::GetReferencedTexture() const
{
	if (Outputs.Num() > 0 && !Outputs[0].OutputName.IsNone())
	{
		return UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, Outputs[0].OutputName, nullptr);
	}
	return nullptr;
}

bool UMaterialExpressionTextureGraphOutput::CanReferenceTexture() const
{
	return true;
}

bool UMaterialExpressionTextureGraphOutput::IsAllowedIn(const UObject* MaterialOrFunction) const
{
	return Cast<const UMaterial>(MaterialOrFunction) != nullptr;
}

#if WITH_EDITOR
// Installed UE builds do not export the MIR::FEmitter methods needed for external
// modules to implement Build(), so this node currently relies on the legacy Compile() path.
int32 UMaterialExpressionTextureGraphOutput::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	FString ErrorMessage;
	FName TargetOutputName = Outputs.IsValidIndex(OutputIndex) ? Outputs[OutputIndex].OutputName : NAME_None;
	UTexture* Texture = UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, TargetOutputName, &ErrorMessage);
	if (!Texture)
	{
		return CompilerError(Compiler, *ErrorMessage);
	}

	const EMaterialSamplerType SamplerType = GetSamplerTypeForResolvedTexture(Texture);
	return SamplerType == SAMPLERTYPE_External ? Compiler->ExternalTexture(Texture) : Compiler->Texture(Texture, SamplerType);
}

void UMaterialExpressionTextureGraphOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString::Printf(TEXT("Graph: %s"), TextureGraphAsset ? *TextureGraphAsset->GetName() : TEXT("Unassigned")));
	OutCaptions.Add(GetNodeTitleText().ToString());
}

FText UMaterialExpressionTextureGraphOutput::GetKeywords() const
{
	return LOCTEXT("TextureGraphOutputKeywords", "texture graph output export bridge material texture object");
}

EMaterialValueType UMaterialExpressionTextureGraphOutput::GetOutputValueType(int32 OutputIndex)
{
	FName TargetOutputName = Outputs.IsValidIndex(OutputIndex) ? Outputs[OutputIndex].OutputName : NAME_None;
	if (const UTexture* Texture = UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, TargetOutputName, nullptr))
	{
		if (Cast<UTextureCube>(Texture) != nullptr)
		{
			return MCT_TextureCube;
		}
		if (Cast<UTexture2DArray>(Texture) != nullptr)
		{
			return MCT_Texture2DArray;
		}
		if (Cast<UTextureCubeArray>(Texture) != nullptr)
		{
			return MCT_TextureCubeArray;
		}
		if (Cast<UVolumeTexture>(Texture) != nullptr)
		{
			return MCT_VolumeTexture;
		}
	}

	return MCT_Texture2D;
}

TSharedPtr<SGraphNodeMaterialBase> UMaterialExpressionTextureGraphOutput::CreateCustomGraphNodeWidget()
{
	if (FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphOutputNodeWidget().IsBound())
	{
		return FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphOutputNodeWidget().Execute(this);
	}

	return nullptr;
}

FText UMaterialExpressionTextureGraphOutput::GetCreationName() const
{
	return GetNodeTitleText();
}

FText UMaterialExpressionTextureGraphOutput::GetCreationDescription() const
{
	return LOCTEXT("TextureGraphOutputCreationDescription", "Use an exported Texture Graph output as a texture object inside a material.");
}

void UMaterialExpressionTextureGraphOutput::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateOutputs();

	if (GraphNode)
	{
		GraphNode->ReconstructNode();
	}
}

void UMaterialExpressionTextureGraphOutput::PostLoad()
{
	Super::PostLoad();
	UpdateOutputs();
}

void UMaterialExpressionTextureGraphOutput::UpdateOutputs()
{
	Outputs.Reset();

	TArray<FName> OutputNames;
	UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraphAsset, OutputNames);
	for (FName OutputName : OutputNames)
	{
		Outputs.Add(FExpressionOutput(OutputName));
	}

	if (Outputs.IsEmpty())
	{
		Outputs.Add(FExpressionOutput(TEXT("")));
	}
}

ETextureGraphOutputNodeStatus UMaterialExpressionTextureGraphOutput::GetOutputNodeStatus(int32& OutReadyCount, int32& OutOutputCount) const
{
	OutReadyCount = 0;
	OutOutputCount = 0;

	if (!TextureGraphAsset)
	{
		return ETextureGraphOutputNodeStatus::NoGraphAssigned;
	}

	TArray<FName> OutputNames;
	UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraphAsset, OutputNames);
	OutOutputCount = OutputNames.Num();

	if (OutOutputCount == 0)
	{
		return ETextureGraphOutputNodeStatus::NoOutputsFound;
	}

	for (const FName OutputName : OutputNames)
	{
		if (UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, OutputName, nullptr))
		{
			++OutReadyCount;
		}
	}

	if (OutReadyCount == OutOutputCount)
	{
		return ETextureGraphOutputNodeStatus::Ready;
	}

	if (OutReadyCount > 0)
	{
		return ETextureGraphOutputNodeStatus::Partial;
	}

	return ETextureGraphOutputNodeStatus::MissingExport;
}

UEdGraphNode* UMaterialExpressionTextureGraphOutput::GetEditorGraphNode() const
{
	return GraphNode;
}
#endif

#if WITH_EDITOR
EMaterialSamplerType UMaterialExpressionTextureGraphOutput::GetSamplerTypeForResolvedTexture(const UTexture* Texture)
{
	return UMaterialExpressionTextureBase::GetSamplerTypeForTexture(Texture);
}

FText UMaterialExpressionTextureGraphOutput::GetNodeTitleText()
{
	return LOCTEXT("TextureGraphOutputNodeTitle", "Texture Graph Output");
}
#endif

#undef LOCTEXT_NAMESPACE
