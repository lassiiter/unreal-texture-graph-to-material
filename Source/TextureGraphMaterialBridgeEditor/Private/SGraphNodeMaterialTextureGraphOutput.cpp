#include "SGraphNodeMaterialTextureGraphOutput.h"

#include "MaterialExpressionTextureGraphOutput.h"
#include "MaterialGraph/MaterialGraphNode.h"
#include "Styling/AppStyle.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#include "Editor.h"

#define LOCTEXT_NAMESPACE "SGraphNodeMaterialTextureGraphOutput"

void SGraphNodeMaterialTextureGraphOutput::Construct(const FArguments& InArgs, UMaterialGraphNode* InNode)
{
	GraphNode = InNode;
	MaterialNode = InNode;
	UpdateGraphNode();
}

void SGraphNodeMaterialTextureGraphOutput::CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox)
{
	SGraphNodeMaterialBase::CreateBelowPinControls(MainBox);

	MainBox->AddSlot()
	.AutoHeight()
	.Padding(6.0f, 4.0f, 6.0f, 2.0f)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(this, &SGraphNodeMaterialTextureGraphOutput::GetStatusLabel)
			.Font(FAppStyle::GetFontStyle("SmallFont"))
			.ColorAndOpacity(this, &SGraphNodeMaterialTextureGraphOutput::GetStatusColor)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(this, &SGraphNodeMaterialTextureGraphOutput::GetOpenHintLabel)
			.Font(FAppStyle::GetFontStyle("SmallFont"))
			.ColorAndOpacity(FLinearColor(0.95f, 0.82f, 0.36f))
			.Visibility(this, &SGraphNodeMaterialTextureGraphOutput::GetOpenHintVisibility)
		]
	];
}

FReply SGraphNodeMaterialTextureGraphOutput::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (const UMaterialExpressionTextureGraphOutput* Expression = GetTextureGraphExpression())
	{
		if (Expression->TextureGraphAsset && GEditor)
		{
			if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				AssetEditorSubsystem->OpenEditorForAsset(Expression->TextureGraphAsset);
				return FReply::Handled();
			}
		}
	}

	return SGraphNodeMaterialBase::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

const UMaterialExpressionTextureGraphOutput* SGraphNodeMaterialTextureGraphOutput::GetTextureGraphExpression() const
{
	return MaterialNode ? Cast<UMaterialExpressionTextureGraphOutput>(MaterialNode->MaterialExpression) : nullptr;
}

FText SGraphNodeMaterialTextureGraphOutput::GetStatusLabel() const
{
	if (const UMaterialExpressionTextureGraphOutput* Expression = GetTextureGraphExpression())
	{
		int32 ReadyOutputCount = 0;
		int32 TotalOutputCount = 0;

		switch (Expression->GetOutputNodeStatus(ReadyOutputCount, TotalOutputCount))
		{
		case ETextureGraphOutputNodeStatus::NoGraphAssigned:
			return LOCTEXT("OutputStatusNoGraph", "No graph assigned");

		case ETextureGraphOutputNodeStatus::NoOutputsFound:
			return LOCTEXT("OutputStatusNoOutputs", "No outputs found");

		case ETextureGraphOutputNodeStatus::Ready:
			return FText::FromString(FString::Printf(TEXT("Ready: %d output%s"), TotalOutputCount, TotalOutputCount == 1 ? TEXT("") : TEXT("s")));

		case ETextureGraphOutputNodeStatus::Partial:
			return FText::FromString(FString::Printf(TEXT("Partial: %d/%d ready"), ReadyOutputCount, TotalOutputCount));

		case ETextureGraphOutputNodeStatus::MissingExport:
			return FText::FromString(FString::Printf(TEXT("Missing export: %d output%s"), TotalOutputCount, TotalOutputCount == 1 ? TEXT("") : TEXT("s")));

		default:
			break;
		}
	}

	return FText::GetEmpty();
}

FSlateColor SGraphNodeMaterialTextureGraphOutput::GetStatusColor() const
{
	if (const UMaterialExpressionTextureGraphOutput* Expression = GetTextureGraphExpression())
	{
		int32 ReadyOutputCount = 0;
		int32 TotalOutputCount = 0;

		switch (Expression->GetOutputNodeStatus(ReadyOutputCount, TotalOutputCount))
		{
		case ETextureGraphOutputNodeStatus::Ready:
			return FSlateColor(FLinearColor(0.43f, 0.82f, 0.50f));

		case ETextureGraphOutputNodeStatus::Partial:
			return FSlateColor(FLinearColor(0.95f, 0.82f, 0.36f));

		case ETextureGraphOutputNodeStatus::NoGraphAssigned:
		case ETextureGraphOutputNodeStatus::NoOutputsFound:
		case ETextureGraphOutputNodeStatus::MissingExport:
			return FSlateColor(FLinearColor(0.95f, 0.55f, 0.36f));

		default:
			break;
		}
	}

	return FSlateColor::UseSubduedForeground();
}

FText SGraphNodeMaterialTextureGraphOutput::GetOpenHintLabel() const
{
	return LOCTEXT("OpenHint", "Double-click to open the Texture Graph source");
}

EVisibility SGraphNodeMaterialTextureGraphOutput::GetOpenHintVisibility() const
{
	if (const UMaterialExpressionTextureGraphOutput* Expression = GetTextureGraphExpression())
	{
		return Expression->TextureGraphAsset ? EVisibility::Visible : EVisibility::Collapsed;
	}

	return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
