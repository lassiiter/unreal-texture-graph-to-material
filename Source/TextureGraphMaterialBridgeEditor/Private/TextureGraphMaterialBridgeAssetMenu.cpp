#include "TextureGraphMaterialBridgeEditorModule.h"

#include "TextureGraphMaterialBridgeMaterialCreationService.h"
#include "ContentBrowserMenuContexts.h"
#include "Misc/DelayedAutoRegister.h"
#include "TextureGraph.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "TextureGraphMaterialBridgeAssetMenu"

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

	static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, []
	{
		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
		{
			FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);

			AddCreateLinkedMaterialMenuEntry(UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UTextureGraph::StaticClass()));
			AddCreateLinkedMaterialMenuEntry(UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UTextureGraphInstance::StaticClass()));
		}));
	});
}

#undef LOCTEXT_NAMESPACE
