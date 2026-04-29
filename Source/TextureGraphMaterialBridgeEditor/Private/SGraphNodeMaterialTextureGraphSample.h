#pragma once

#include "MaterialEditor/MaterialNodes/SGraphNodeMaterialBase.h"

class UMaterialExpressionTextureGraphSample;
class UMaterialGraphNode;

class SGraphNodeMaterialTextureGraphSample : public SGraphNodeMaterialBase
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeMaterialTextureGraphSample) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UMaterialGraphNode* InNode);

protected:
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

private:
	const UMaterialExpressionTextureGraphSample* GetTextureGraphExpression() const;
};
