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

		TArray<UTextureGraph*> TextureGraphs = Context->LoadSelectedObjects<UTextureGraph>();
		if (TextureGraphs.IsEmpty())
		{
			return;
		}

		FTextureGraphMaterialBridgeEditorModule& Module = FModuleManager::LoadModuleChecked<FTextureGraphMaterialBridgeEditorModule>(TEXT("TextureGraphMaterialBridgeEditor"));
		Module.GetMaterialCreationService().CreateLinkedMaterials(TextureGraphs);
	}

	static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, []
	{
		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
		{
			FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);

			UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UTextureGraph::StaticClass());
			FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
			Section.AddDynamicEntry(NAME_None, FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
			{
				if (UContentBrowserAssetContextMenuContext::FindContextWithAssets(InSection) == nullptr)
				{
					return;
				}

				const TAttribute<FText> Label = LOCTEXT("CreateLinkedMaterialLabel", "Create Linked Material");
				const TAttribute<FText> ToolTip = LOCTEXT("CreateLinkedMaterialToolTip", "Create a linked material beside this Texture Graph and auto-wire supported outputs.");
				const FToolMenuExecuteAction UIAction = FToolMenuExecuteAction::CreateStatic(&ExecuteCreateLinkedMaterial);
				InSection.AddMenuEntry("TextureGraphMaterialBridge_CreateLinkedMaterial", Label, ToolTip, FSlateIcon(), UIAction);
			}));
		}));
	});
}

#undef LOCTEXT_NAMESPACE
