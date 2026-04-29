#pragma once

#include "MaterialEditor/MaterialNodes/SGraphNodeMaterialBase.h"

class UMaterialExpressionTextureGraphOutput;
class UMaterialGraphNode;

class SGraphNodeMaterialTextureGraphOutput : public SGraphNodeMaterialBase
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeMaterialTextureGraphOutput) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UMaterialGraphNode* InNode);

protected:
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

private:
	const UMaterialExpressionTextureGraphOutput* GetTextureGraphExpression() const;
	FText GetStatusLabel() const;
	FSlateColor GetStatusColor() const;
	FText GetOpenHintLabel() const;
	EVisibility GetOpenHintVisibility() const;
};
