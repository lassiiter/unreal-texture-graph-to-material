#include "TextureGraphMaterialBridgeEditorModule.h"

#include "TextureGraphMaterialBridgeMaterialInstanceBindingService.h"
#include "TextureGraphMaterialBridgeMaterialCreationService.h"
#include "ContentBrowserMenuContexts.h"
#include "IMaterialEditor.h"
#include "MaterialEditorContext.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/DelayedAutoRegister.h"
#include "TextureGraph.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "TextureGraphMaterialBridgeAssetMenu"

DEFINE_LOG_CATEGORY_STATIC(LogTextureGraphMaterialBridgeAssetMenu, Log, All);

namespace TextureGraphMaterialBridgeAssetMenu
{
	static void ExecuteCreateLinkedMaterial(const FToolMenuContext& InContext)
	{
		const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(InContext);
		if (!Context)
		{
			return;
		}

		TArray<UTextureGraphBase*> TextureGraphSources;
		for (UTextureGraph* TextureGraph : Context->LoadSelectedObjects<UTextureGraph>())
		{
			TextureGraphSources.AddUnique(TextureGraph);
		}
		for (UTextureGraphInstance* TextureGraphInstance : Context->LoadSelectedObjects<UTextureGraphInstance>())
		{
			TextureGraphSources.AddUnique(TextureGraphInstance);
		}

		if (TextureGraphSources.IsEmpty())
		{
			return;
		}

		FTextureGraphMaterialBridgeEditorModule& Module = FModuleManager::LoadModuleChecked<FTextureGraphMaterialBridgeEditorModule>(TEXT("TextureGraphMaterialBridgeEditor"));
		Module.GetMaterialCreationService().CreateLinkedMaterials(TextureGraphSources);
	}

	static void ExecuteBindTextureGraphOutput(const FToolMenuContext& InContext)
	{
		const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(InContext);
		if (!Context)
		{
			return;
		}

		TArray<UMaterialInstanceConstant*> MaterialInstances = Context->LoadSelectedObjects<UMaterialInstanceConstant>();
		if (MaterialInstances.IsEmpty())
		{
			UE_LOG(LogTextureGraphMaterialBridgeAssetMenu, Warning, TEXT("Bind Texture Graph Output menu action found no selected Material Instance assets."));
			return;
		}

		UE_LOG(LogTextureGraphMaterialBridgeAssetMenu, Log, TEXT("Executing Bind Texture Graph Output for %d selected Material Instance asset(s)."), MaterialInstances.Num());

		FTextureGraphMaterialBridgeEditorModule& Module = FModuleManager::LoadModuleChecked<FTextureGraphMaterialBridgeEditorModule>(TEXT("TextureGraphMaterialBridgeEditor"));
		for (UMaterialInstanceConstant* MaterialInstance : MaterialInstances)
		{
			Module.GetMaterialInstanceBindingService().OpenBindingDialog(MaterialInstance);
		}
	}

	static void ExecuteBindTextureGraphOutputFromMaterialInstanceEditor(const FToolMenuContext& InContext)
	{
		const UMaterialEditorMenuContext* Context = InContext.FindContext<UMaterialEditorMenuContext>();
		const TSharedPtr<IMaterialEditor> MaterialEditor = Context ? Context->MaterialEditor.Pin() : nullptr;
		UMaterialInstanceConstant* MaterialInstance = MaterialEditor ? Cast<UMaterialInstanceConstant>(MaterialEditor->GetMaterialInterface()) : nullptr;
		if (!MaterialInstance)
		{
			UE_LOG(LogTextureGraphMaterialBridgeAssetMenu, Warning, TEXT("Texture Graph toolbar action could not resolve the current Material Instance from the Material Editor context."));
			return;
		}

		UE_LOG(LogTextureGraphMaterialBridgeAssetMenu, Log, TEXT("Executing Texture Graph toolbar action for Material Instance '%s'."), *MaterialInstance->GetPathName());

		FTextureGraphMaterialBridgeEditorModule& Module = FModuleManager::LoadModuleChecked<FTextureGraphMaterialBridgeEditorModule>(TEXT("TextureGraphMaterialBridgeEditor"));
		Module.GetMaterialInstanceBindingService().OpenBindingDialog(MaterialInstance);
	}

	static void AddCreateLinkedMaterialMenuEntry(UToolMenu* Menu)
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
		Section.AddDynamicEntry(NAME_None, FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
		{
			if (UContentBrowserAssetContextMenuContext::FindContextWithAssets(InSection) == nullptr)
			{
				return;
			}

			const TAttribute<FText> Label = LOCTEXT("CreateLinkedMaterialLabel", "Create Linked Material");
			const TAttribute<FText> ToolTip = LOCTEXT("CreateLinkedMaterialToolTip", "Create a linked material beside this Texture Graph source and auto-wire supported outputs.");
			const FToolMenuExecuteAction UIAction = FToolMenuExecuteAction::CreateStatic(&ExecuteCreateLinkedMaterial);
			InSection.AddMenuEntry("TextureGraphMaterialBridge_CreateLinkedMaterial", Label, ToolTip, FSlateIcon(), UIAction);
		}));
	}

	static void AddBindTextureGraphOutputMenuEntry(UToolMenu* Menu)
	{
		UE_LOG(LogTextureGraphMaterialBridgeAssetMenu, Log, TEXT("Registering Content Browser Bind Texture Graph Output menu entry."));

		FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
		Section.AddDynamicEntry(NAME_None, FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
		{
			if (UContentBrowserAssetContextMenuContext::FindContextWithAssets(InSection) == nullptr)
			{
				return;
			}

			const TAttribute<FText> Label = LOCTEXT("BindTextureGraphOutputLabel", "Bind Texture Graph Output");
			const TAttribute<FText> ToolTip = LOCTEXT("BindTextureGraphOutputToolTip", "Bind a standard 2D texture parameter on this Material Instance to a Texture Graph output.");
			const FToolMenuExecuteAction UIAction = FToolMenuExecuteAction::CreateStatic(&ExecuteBindTextureGraphOutput);
			InSection.AddMenuEntry("TextureGraphMaterialBridge_BindTextureGraphOutput", Label, ToolTip, FSlateIcon(), UIAction);
		}));
	}

	static void AddMaterialInstanceEditorToolbarEntry(UToolMenu* Menu)
	{
		UE_LOG(LogTextureGraphMaterialBridgeAssetMenu, Log, TEXT("Registering ToolMenus Material Instance editor Texture Graph toolbar entry."));

		FToolMenuSection& Section = Menu->FindOrAddSection("MaterialInstanceTools");
		const TAttribute<FText> Label = LOCTEXT("BindTextureGraphToolbarLabel", "Texture Graph");
		const TAttribute<FText> ToolTip = LOCTEXT("BindTextureGraphToolbarToolTip", "Bind a Material Instance texture parameter to a Texture Graph output.");
		const FToolMenuExecuteAction UIAction = FToolMenuExecuteAction::CreateStatic(&ExecuteBindTextureGraphOutputFromMaterialInstanceEditor);
		Section.AddEntry(FToolMenuEntry::InitToolBarButton("TextureGraphMaterialBridge_BindTextureGraphOutput", UIAction, Label, ToolTip, FSlateIcon()));
	}

	static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, []
	{
		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
		{
			FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);

			AddCreateLinkedMaterialMenuEntry(UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UTextureGraph::StaticClass()));
			AddCreateLinkedMaterialMenuEntry(UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UTextureGraphInstance::StaticClass()));
			AddBindTextureGraphOutputMenuEntry(UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UMaterialInstanceConstant::StaticClass()));
			AddMaterialInstanceEditorToolbarEntry(UToolMenus::Get()->ExtendMenu("AssetEditor.MaterialInstanceEditorApp.ToolBar"));
		}));
	});
}

#undef LOCTEXT_NAMESPACE
