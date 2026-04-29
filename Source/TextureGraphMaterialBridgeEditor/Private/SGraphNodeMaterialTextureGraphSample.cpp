#include "SGraphNodeMaterialTextureGraphSample.h"

#include "MaterialExpressionTextureGraphSample.h"
#include "MaterialGraph/MaterialGraphNode.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "Editor.h"

void SGraphNodeMaterialTextureGraphSample::Construct(const FArguments& InArgs, UMaterialGraphNode* InNode)
{
	GraphNode = InNode;
	MaterialNode = InNode;
	UpdateGraphNode();
}

FReply SGraphNodeMaterialTextureGraphSample::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (const UMaterialExpressionTextureGraphSample* Expression = GetTextureGraphExpression())
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

const UMaterialExpressionTextureGraphSample* SGraphNodeMaterialTextureGraphSample::GetTextureGraphExpression() const
{
	return MaterialNode ? Cast<UMaterialExpressionTextureGraphSample>(MaterialNode->MaterialExpression) : nullptr;
}
