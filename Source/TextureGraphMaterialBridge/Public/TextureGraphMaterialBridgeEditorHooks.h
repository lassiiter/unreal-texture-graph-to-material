#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

#if WITH_EDITOR
class SGraphNodeMaterialBase;
class UMaterialExpressionTextureGraphOutput;
class UMaterialExpressionTextureGraphSample;

class TEXTUREGRAPHMATERIALBRIDGE_API FTextureGraphMaterialBridgeEditorHooks
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SGraphNodeMaterialBase>, FCreateTextureGraphOutputNodeWidget, UMaterialExpressionTextureGraphOutput*);
	DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SGraphNodeMaterialBase>, FCreateTextureGraphSampleNodeWidget, UMaterialExpressionTextureGraphSample*);

	static FCreateTextureGraphOutputNodeWidget& OnCreateTextureGraphOutputNodeWidget();
	static FCreateTextureGraphSampleNodeWidget& OnCreateTextureGraphSampleNodeWidget();
};
#endif
