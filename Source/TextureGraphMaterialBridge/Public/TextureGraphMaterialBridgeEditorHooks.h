#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

#if WITH_EDITOR
class SGraphNodeMaterialBase;
class UMaterialExpressionTextureGraphOutput;

class TEXTUREGRAPHMATERIALBRIDGE_API FTextureGraphMaterialBridgeEditorHooks
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SGraphNodeMaterialBase>, FCreateTextureGraphOutputNodeWidget, UMaterialExpressionTextureGraphOutput*);

	static FCreateTextureGraphOutputNodeWidget& OnCreateTextureGraphOutputNodeWidget();
};
#endif
