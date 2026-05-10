#include "TextureGraphMaterialBridgeMaterialInstanceBindings.h"

#include "TextureGraph.h"

bool FTextureGraphMaterialInstanceParameterBinding::MatchesParameter(const FMaterialParameterInfo& InParameterInfo) const
{
	return ParameterInfo.Name == InParameterInfo.Name
		&& ParameterInfo.Association == InParameterInfo.Association
		&& ParameterInfo.Index == InParameterInfo.Index;
}

bool FTextureGraphMaterialInstanceParameterBinding::ReferencesTextureGraph(const UTextureGraphBase* TextureGraph) const
{
	if (!TextureGraph)
	{
		return false;
	}

	return TextureGraphPath == FSoftObjectPath(TextureGraph);
}

FTextureGraphMaterialInstanceParameterBinding* UTextureGraphMaterialInstanceBindingsAssetUserData::FindBinding(const FMaterialParameterInfo& ParameterInfo)
{
	return Bindings.FindByPredicate([&ParameterInfo](const FTextureGraphMaterialInstanceParameterBinding& Binding)
	{
		return Binding.MatchesParameter(ParameterInfo);
	});
}

const FTextureGraphMaterialInstanceParameterBinding* UTextureGraphMaterialInstanceBindingsAssetUserData::FindBinding(const FMaterialParameterInfo& ParameterInfo) const
{
	return Bindings.FindByPredicate([&ParameterInfo](const FTextureGraphMaterialInstanceParameterBinding& Binding)
	{
		return Binding.MatchesParameter(ParameterInfo);
	});
}

FTextureGraphMaterialInstanceParameterBinding& UTextureGraphMaterialInstanceBindingsAssetUserData::AddOrUpdateBinding(const FTextureGraphMaterialInstanceParameterBinding& Binding)
{
	if (FTextureGraphMaterialInstanceParameterBinding* ExistingBinding = FindBinding(Binding.ParameterInfo))
	{
		*ExistingBinding = Binding;
		return *ExistingBinding;
	}

	return Bindings.Add_GetRef(Binding);
}

bool UTextureGraphMaterialInstanceBindingsAssetUserData::RemoveBinding(const FMaterialParameterInfo& ParameterInfo)
{
	const int32 RemovedCount = Bindings.RemoveAll([&ParameterInfo](const FTextureGraphMaterialInstanceParameterBinding& Binding)
	{
		return Binding.MatchesParameter(ParameterInfo);
	});

	return RemovedCount > 0;
}

bool UTextureGraphMaterialInstanceBindingsAssetUserData::ReferencesTextureGraph(const UTextureGraphBase* TextureGraph) const
{
	if (!TextureGraph)
	{
		return false;
	}

	return Bindings.ContainsByPredicate([TextureGraph](const FTextureGraphMaterialInstanceParameterBinding& Binding)
	{
		return Binding.bEnabled && Binding.ReferencesTextureGraph(TextureGraph);
	});
}

bool UTextureGraphMaterialInstanceBindingsAssetUserData::IsEmpty() const
{
	return Bindings.IsEmpty();
}
