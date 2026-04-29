#pragma once

#include "CoreMinimal.h"

class UTextureGraph;

class FTextureGraphMaterialBridgeMaterialCreationService
{
public:
	void CreateLinkedMaterials(TConstArrayView<UTextureGraph*> TextureGraphs) const;
};
