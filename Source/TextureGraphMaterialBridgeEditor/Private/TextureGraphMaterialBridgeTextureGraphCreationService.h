#pragma once

#include "CoreMinimal.h"

class UTexture;

class FTextureGraphMaterialBridgeTextureGraphCreationService
{
public:
	void CreateTextureGraphFromTextures(TConstArrayView<UTexture*> Textures) const;
};
