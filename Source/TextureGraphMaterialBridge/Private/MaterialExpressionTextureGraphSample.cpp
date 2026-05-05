#include "MaterialExpressionTextureGraphSample.h"

#include "TextureGraphMaterialBridgeEditorHooks.h"
#include "TextureGraphMaterialBridgeExpressionUtils.h"
#include "EdGraph/EdGraphNode.h"
#include "EditorSupportDelegates.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Expressions/Output/TG_Expression_Output.h"
#include "MaterialCompiler.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionTextureBase.h"

#define LOCTEXT_NAMESPACE "MaterialExpressionTextureGraphSample"

UMaterialExpressionTextureGraphSample::UMaterialExpressionTextureGraphSample(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ResetMenuCategories();

#if WITH_EDITORONLY_DATA
	bShowTextureInputPin = false;
#endif
}

TArray<FString> UMaterialExpressionTextureGraphSample::GetAvailableOutputNameOptions() const
{
	TArray<FName> OutputNames;
	UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraphAsset, OutputNames);

	TArray<FString> OutputNameOptions;
	OutputNameOptions.Reserve(OutputNames.Num());
	for (const FName OutputName : OutputNames)
	{
		OutputNameOptions.Add(OutputName.ToString());
	}

	return OutputNameOptions;
}

bool UMaterialExpressionTextureGraphSample::ReferencesTextureGraph(const UTextureGraphBase* InTextureGraph) const
{
	return TextureGraphAsset == InTextureGraph;
}

UObject* UMaterialExpressionTextureGraphSample::GetReferencedTexture() const
{
	return UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, TargetOutputName, nullptr);
}

bool UMaterialExpressionTextureGraphSample::CanReferenceTexture() const
{
	return true;
}

bool UMaterialExpressionTextureGraphSample::IsAllowedIn(const UObject* MaterialOrFunction) const
{
	return Cast<const UMaterial>(MaterialOrFunction) != nullptr;
}

void UMaterialExpressionTextureGraphSample::PostInitProperties()
{
	Super::PostInitProperties();
	ResetMenuCategories();
}

void UMaterialExpressionTextureGraphSample::ResetMenuCategories()
{
	MenuCategories.Reset();
	MenuCategories.Add(LOCTEXT("TextureGraphCategory", "Texture Graph"));
}

#if WITH_EDITOR
bool UMaterialExpressionTextureGraphSample::CanEditChange(const FProperty* InProperty) const
{
	bool bIsEditable = Super::CanEditChange(InProperty);
	if (bIsEditable && InProperty && InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UMaterialExpressionTextureBase, Texture))
	{
		bIsEditable = false;
	}

	return bIsEditable;
}

void UMaterialExpressionTextureGraphSample::PostLoad()
{
	Super::PostLoad();
	ResetMenuCategories();
	RefreshOutputSelection();
	SyncResolvedTexture();
}

int32 UMaterialExpressionTextureGraphSample::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	RefreshOutputSelection();

	FString ErrorMessage;
	UTexture* ResolvedTexture = UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, TargetOutputName, &ErrorMessage);
	if (!ResolvedTexture)
	{
		return CompilerError(Compiler, *ErrorMessage);
	}

	UTexture2D* ResolvedTexture2D = Cast<UTexture2D>(ResolvedTexture);
	if (!ResolvedTexture2D)
	{
		ErrorMessage = UE::TextureGraphMaterialBridge::BuildTextureGraphMaterialUsageError(
			FString::Printf(
				TEXT("Texture Graph Sample currently supports only exported UTexture2D assets, but the resolved asset '%s' is a %s."),
				*ResolvedTexture->GetName(),
				*ResolvedTexture->GetClass()->GetName()),
			TextureGraphAsset,
			TargetOutputName);
		return CompilerError(Compiler, *ErrorMessage);
	}

	if (Texture != ResolvedTexture2D)
	{
		Texture = ResolvedTexture2D;
		AutoSetSampleType();
	}

	return Super::Compile(Compiler, OutputIndex);
}

void UMaterialExpressionTextureGraphSample::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString::Printf(TEXT("Output: %s"), TargetOutputName.IsNone() ? TEXT("Unassigned") : *TargetOutputName.ToString()));
	OutCaptions.Add(FString::Printf(TEXT("Source: %s"), TextureGraphAsset ? *TextureGraphAsset->GetName() : TEXT("Unassigned")));
	OutCaptions.Add(GetNodeTitleText().ToString());
}

FText UMaterialExpressionTextureGraphSample::GetKeywords() const
{
	return LOCTEXT("TextureGraphSampleKeywords", "texture graph instance sample export bridge material texture sampler");
}

TSharedPtr<SGraphNodeMaterialBase> UMaterialExpressionTextureGraphSample::CreateCustomGraphNodeWidget()
{
	if (FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphSampleNodeWidget().IsBound())
	{
		return FTextureGraphMaterialBridgeEditorHooks::OnCreateTextureGraphSampleNodeWidget().Execute(this);
	}

	return nullptr;
}

FText UMaterialExpressionTextureGraphSample::GetCreationName() const
{
	return GetNodeTitleText();
}

FText UMaterialExpressionTextureGraphSample::GetCreationDescription() const
{
	return LOCTEXT("TextureGraphSampleCreationDescription", "Sample one exported Texture Graph output like a standard 2D texture sample.");
}

void UMaterialExpressionTextureGraphSample::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();
	const bool bTextureGraphPropertyChanged =
		PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, TextureGraphAsset) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, TargetOutputName);

	if (bTextureGraphPropertyChanged)
	{
		RefreshOutputSelection();
	}

	SyncResolvedTexture();

	if (GraphNode && bTextureGraphPropertyChanged)
	{
		GraphNode->ReconstructNode();
	}

	if (bTextureGraphPropertyChanged)
	{
		FEditorSupportDelegates::ForcePropertyWindowRebuild.Broadcast(this);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

UEdGraphNode* UMaterialExpressionTextureGraphSample::GetEditorGraphNode() const
{
	return GraphNode;
}

void UMaterialExpressionTextureGraphSample::RefreshResolvedTextureGraphOutput()
{
	RefreshOutputSelection();
	SyncResolvedTexture(true, true);

	if (GraphNode)
	{
		GraphNode->ReconstructNode();
	}
}

void UMaterialExpressionTextureGraphSample::RefreshOutputSelection()
{
	TArray<FName> OutputNames;
	UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraphAsset, OutputNames);

	if (OutputNames.IsEmpty())
	{
		TargetOutputName = NAME_None;
		return;
	}

	if (OutputNames.Contains(TargetOutputName))
	{
		return;
	}

	if (const UTG_Expression_Output* OutputExpression = UE::TextureGraphMaterialBridge::FindTextureGraphOutputExpression(TextureGraphAsset, TargetOutputName, nullptr))
	{
		TargetOutputName = UE::TextureGraphMaterialBridge::GetTextureGraphOutputName(*OutputExpression);
		return;
	}

	TargetOutputName = OutputNames[0];
}

void UMaterialExpressionTextureGraphSample::SyncResolvedTexture(bool bForceSamplerTypeRefresh, bool bForcePropertyWindowRebuild)
{
	UTexture* ResolvedTexture = UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraphAsset, TargetOutputName, nullptr);
	const bool bTextureChanged = Texture != ResolvedTexture;

	Texture = ResolvedTexture;

	if ((bTextureChanged || bForceSamplerTypeRefresh) && Texture)
	{
		AutoSetSampleType();
	}

	if (bTextureChanged || bForcePropertyWindowRebuild)
	{
		FEditorSupportDelegates::ForcePropertyWindowRebuild.Broadcast(this);
	}
}

FText UMaterialExpressionTextureGraphSample::GetNodeTitleText()
{
	return LOCTEXT("TextureGraphSampleNodeTitle", "Texture Graph Sample");
}
#endif

#undef LOCTEXT_NAMESPACE
