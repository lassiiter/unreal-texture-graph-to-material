#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialParameters.h"
#include "UObject/SoftObjectPath.h"

struct FPropertyRowExtensionButton;
class UDEditorTextureParameterValue;
class UMaterialEditorInstanceConstant;
class UMaterialInstanceConstant;
class UTextureGraphBase;
struct FOnGenerateGlobalRowExtensionArgs;
struct FTextureGraphMaterialInstanceParameterBinding;

class FTextureGraphMaterialBridgeMaterialInstanceBindingService
{
public:
	void Startup();
	void Shutdown();

	TArray<FMaterialParameterInfo> ListEligibleTextureParameters(UMaterialInstanceConstant* MaterialInstance) const;
	void OpenBindingDialog(UMaterialInstanceConstant* MaterialInstance) const;
	TArray<FSoftObjectPath> FindBoundMaterialInstancePaths(const UTextureGraphBase* TextureGraph) const;
	void RefreshBindingsForTextureGraph(UTextureGraphBase* TextureGraph, const TArray<FSoftObjectPath>& MaterialInstancePaths) const;

private:
	TSharedRef<class FExtender> CreateMaterialInstanceToolbarExtender(const TSharedRef<class FUICommandList> CommandList, const TArray<UObject*> ContextObjects);
	void FillMaterialInstanceToolbar(class FToolBarBuilder& ToolbarBuilder, TWeakObjectPtr<UMaterialInstanceConstant> MaterialInstance) const;
	void HandleGenerateGlobalRowExtension(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensions);

	void BindFromPicker(UDEditorTextureParameterValue* TextureParameter, UMaterialEditorInstanceConstant* EditorInstance) const;
	void BindTextureGraphOutput(UMaterialInstanceConstant* MaterialInstance, const FMaterialParameterInfo& ParameterInfo, UTextureGraphBase* TextureGraph, FName OutputName, UDEditorTextureParameterValue* TextureParameter, UMaterialEditorInstanceConstant* EditorInstance) const;
	void RefreshBinding(UMaterialInstanceConstant* MaterialInstance, const FTextureGraphMaterialInstanceParameterBinding& Binding, UDEditorTextureParameterValue* TextureParameter = nullptr, UMaterialEditorInstanceConstant* EditorInstance = nullptr) const;
	void ClearBinding(UMaterialInstanceConstant* MaterialInstance, const FMaterialParameterInfo& ParameterInfo, UDEditorTextureParameterValue* TextureParameter, UMaterialEditorInstanceConstant* EditorInstance) const;

	FDelegateHandle RowExtensionHandle;
	FDelegateHandle MaterialEditorToolbarExtenderHandle;
};
