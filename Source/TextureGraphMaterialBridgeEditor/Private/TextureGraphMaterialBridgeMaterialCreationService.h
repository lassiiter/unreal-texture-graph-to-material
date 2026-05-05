#pragma once

#include "CoreMinimal.h"

class UTextureGraphBase;

class FTextureGraphMaterialBridgeMaterialCreationService
{
public:
	void CreateLinkedMaterials(TConstArrayView<UTextureGraphBase*> TextureGraphs) const;
};
