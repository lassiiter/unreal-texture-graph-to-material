#include "TextureGraphMaterialBridgeTextureGraphCreationService.h"

#include "2D/TextureHelper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Expressions/Input/TG_Expression_Texture.h"
#include "Expressions/Output/TG_Expression_Output.h"
#include "FileHelpers.h"
#include "Framework/Notifications/NotificationManager.h"
#include "IAssetTools.h"
#include "Misc/PackageName.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TG_Graph.h"
#include "TG_Node.h"
#include "TG_Pin.h"
#include "TextureGraph.h"
#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY_STATIC(LogTextureGraphMaterialBridgeTextureGraphCreationService, Log, All);

namespace UE::TextureGraphMaterialBridgeEditor
{
	namespace
	{
		constexpr int32 TextureNodePosX = -500;
		constexpr int32 OutputNodePosX = 100;
		constexpr int32 FirstNodePosY = 0;
		constexpr int32 RowSpacingY = 220;

		void ShowNotification(const FText& Message, SNotificationItem::ECompletionState CompletionState)
		{
			FNotificationInfo Info(Message);
			Info.bUseSuccessFailIcons = true;
			Info.ExpireDuration = 5.0f;
			Info.FadeOutDuration = 0.2f;

			if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info))
			{
				Notification->SetCompletionState(CompletionState);
			}
		}

		void NotifyWarning(const FText& Message)
		{
			UE_LOG(LogTextureGraphMaterialBridgeTextureGraphCreationService, Warning, TEXT("%s"), *Message.ToString());
			ShowNotification(Message, SNotificationItem::CS_Fail);
		}

		void NotifySuccess(const FText& Message)
		{
			UE_LOG(LogTextureGraphMaterialBridgeTextureGraphCreationService, Display, TEXT("%s"), *Message.ToString());
			ShowNotification(Message, SNotificationItem::CS_Success);
		}

		TArray<UTexture*> FilterSupportedTextures(TConstArrayView<UTexture*> Textures)
		{
			TArray<UTexture*> SupportedTextures;
			for (UTexture* Texture : Textures)
			{
				if (!Texture || Texture->HasAnyFlags(RF_ClassDefaultObject))
				{
					continue;
				}

				if (!TextureHelper::CanSupportTexture(Texture))
				{
					UE_LOG(LogTextureGraphMaterialBridgeTextureGraphCreationService, Warning, TEXT("Skipping unsupported texture '%s'."), *Texture->GetPathName());
					continue;
				}

				SupportedTextures.AddUnique(Texture);
			}

			return SupportedTextures;
		}

		FName MakeUniqueOutputName(const UTexture* Texture, TSet<FName>& UsedOutputNames)
		{
			const FString BaseName = Texture ? Texture->GetName() : TEXT("Texture");
			FName OutputName(*BaseName);

			if (!UsedOutputNames.Contains(OutputName))
			{
				UsedOutputNames.Add(OutputName);
				return OutputName;
			}

			for (int32 Index = 2;; ++Index)
			{
				OutputName = FName(*FString::Printf(TEXT("%s_%d"), *BaseName, Index));
				if (!UsedOutputNames.Contains(OutputName))
				{
					UsedOutputNames.Add(OutputName);
					return OutputName;
				}
			}
		}

		void SetNodePosition(UTG_Node* Node, int32 PosX, int32 PosY)
		{
			if (!Node)
			{
				return;
			}

			Node->EditorData.PosX = PosX;
			Node->EditorData.PosY = PosY;
		}

		UTG_Node* FindFirstOutputNode(UTextureGraph* TextureGraph)
		{
			if (!TextureGraph || !TextureGraph->Graph())
			{
				return nullptr;
			}

			UTG_Node* FoundNode = nullptr;
			TextureGraph->Graph()->ForEachNodes([&FoundNode](const UTG_Node* Node, uint32)
			{
				if (!FoundNode && Node && Cast<UTG_Expression_Output>(Node->GetExpression()))
				{
					FoundNode = const_cast<UTG_Node*>(Node);
				}
			});

			return FoundNode;
		}

		UTG_Node* CreateOutputNode(UTextureGraph* TextureGraph, bool bCanReuseDefaultOutput)
		{
			if (!TextureGraph || !TextureGraph->Graph())
			{
				return nullptr;
			}

			if (bCanReuseDefaultOutput)
			{
				if (UTG_Node* ExistingOutputNode = FindFirstOutputNode(TextureGraph))
				{
					return ExistingOutputNode;
				}
			}

			UTG_Node* OutputNode = TextureGraph->Graph()->CreateExpressionNode(UTG_Expression_Output::StaticClass());
			if (UTG_Expression_Output* OutputExpression = OutputNode ? Cast<UTG_Expression_Output>(OutputNode->GetExpression()) : nullptr)
			{
				OutputExpression->InitializeOutputSettings();
			}

			return OutputNode;
		}

		void SyncOutputSettingsPin(UTG_Expression_Output* OutputExpression)
		{
			if (!OutputExpression)
			{
				return;
			}

			if (UTG_Node* OutputNode = OutputExpression->GetParentNode())
			{
				if (UTG_Pin* SettingsPin = OutputNode->GetPin(GET_MEMBER_NAME_CHECKED(UTG_Expression_Output, OutputSettings)))
				{
					SettingsPin->FromString(OutputExpression->OutputSettings.ToString());
				}
			}
		}

		void ConfigureOutputExpression(UTG_Expression_Output* OutputExpression, UTextureGraph* TextureGraph, UTexture* SourceTexture, FName OutputName, const FString& PackagePath)
		{
			if (!OutputExpression || !TextureGraph || !SourceTexture)
			{
				return;
			}

			OutputExpression->SetTitleName(OutputName);
			OutputExpression->OutputSettings.Initialize(TextureGraph->GetPathName(), OutputName);
			OutputExpression->OutputSettings.OutputName = OutputName;
			OutputExpression->OutputSettings.BaseName = OutputName;
			OutputExpression->OutputSettings.FolderPath = FName(*PackagePath);
			OutputExpression->OutputSettings.bSRGB = SourceTexture->SRGB;
			OutputExpression->OutputSettings.bShouldExport = true;

			SyncOutputSettingsPin(OutputExpression);
		}

		UTextureGraph* CreateTextureGraphAsset(const TArray<UTexture*>& Textures, FString& OutPackagePath)
		{
			if (Textures.IsEmpty() || !Textures[0])
			{
				return nullptr;
			}

			const FString SourcePackageName = Textures[0]->GetOutermost()->GetName();
			OutPackagePath = FPackageName::GetLongPackagePath(SourcePackageName);

			const FString BasePackageName = FString::Printf(TEXT("%s/TG_%s"), *OutPackagePath, *Textures[0]->GetName());
			FString UniquePackageName;
			FString UniqueAssetName;
			IAssetTools::Get().CreateUniqueAssetName(BasePackageName, TEXT(""), UniquePackageName, UniqueAssetName);

			UPackage* Package = CreatePackage(*UniquePackageName);
			if (!Package)
			{
				return nullptr;
			}

			UTextureGraph* TextureGraph = NewObject<UTextureGraph>(
				Package,
				UTextureGraph::StaticClass(),
				*UniqueAssetName,
				RF_Public | RF_Standalone | RF_Transactional);

			if (!TextureGraph)
			{
				return nullptr;
			}

			TextureGraph->Construct(UniqueAssetName);
			FAssetRegistryModule::AssetCreated(TextureGraph);
			TextureGraph->MarkPackageDirty();

			return TextureGraph;
		}

		bool AddTextureOutputRow(UTextureGraph* TextureGraph, UTexture* Texture, FName OutputName, const FString& PackagePath, int32 RowIndex, bool bCanReuseDefaultOutput)
		{
			if (!TextureGraph || !TextureGraph->Graph() || !Texture)
			{
				return false;
			}

			UTG_Expression_Texture* TextureExpression = NewObject<UTG_Expression_Texture>();
			UTG_Node* TextureNode = TextureGraph->Graph()->CreateExpressionNode(TextureExpression);
			if (!TextureNode || !TextureExpression)
			{
				return false;
			}

			TextureExpression->SetTitleName(OutputName);
			TextureExpression->SetSource(Texture);
			SetNodePosition(TextureNode, TextureNodePosX, FirstNodePosY + RowIndex * RowSpacingY);

			UTG_Node* OutputNode = CreateOutputNode(TextureGraph, bCanReuseDefaultOutput);
			UTG_Expression_Output* OutputExpression = OutputNode ? Cast<UTG_Expression_Output>(OutputNode->GetExpression()) : nullptr;
			if (!OutputNode || !OutputExpression)
			{
				return false;
			}

			ConfigureOutputExpression(OutputExpression, TextureGraph, Texture, OutputName, PackagePath);
			SetNodePosition(OutputNode, OutputNodePosX, FirstNodePosY + RowIndex * RowSpacingY);

			FTG_Name TextureOutputPinName = GET_MEMBER_NAME_CHECKED(UTG_Expression_Texture, Output);
			FTG_Name OutputSourcePinName = GET_MEMBER_NAME_CHECKED(UTG_Expression_Output, Source);
			return TextureGraph->Graph()->Connect(*TextureNode, TextureOutputPinName, *OutputNode, OutputSourcePinName);
		}

		void SaveAndOpenTextureGraph(UTextureGraph* TextureGraph)
		{
			if (!TextureGraph)
			{
				return;
			}

			TextureGraph->UpdateGlobalTGSettings();
			TextureGraph->PostEditChange();
			TextureGraph->MarkPackageDirty();

			TArray<UPackage*> PackagesToSave;
			PackagesToSave.Add(TextureGraph->GetOutermost());
			const bool bSaved = UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);

			if (GEditor)
			{
				if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
				{
					AssetEditorSubsystem->OpenEditorForAsset(TextureGraph);
				}
			}

			const FText SuccessMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "TextureGraphCreatedFromTextures", "Created Texture Graph '{0}'."),
				FText::FromString(TextureGraph->GetPathName()));
			NotifySuccess(SuccessMessage);

			if (!bSaved)
			{
				const FText SaveWarningMessage = FText::Format(
					NSLOCTEXT("TextureGraphMaterialBridge", "TextureGraphCreatedFromTexturesSaveFailed", "Created Texture Graph '{0}', but saving the package failed."),
					FText::FromString(TextureGraph->GetName()));
				NotifyWarning(SaveWarningMessage);
			}
		}
	}
}

void FTextureGraphMaterialBridgeTextureGraphCreationService::CreateTextureGraphFromTextures(TConstArrayView<UTexture*> Textures) const
{
	const TArray<UTexture*> SupportedTextures = UE::TextureGraphMaterialBridgeEditor::FilterSupportedTextures(Textures);
	if (SupportedTextures.IsEmpty())
	{
		UE::TextureGraphMaterialBridgeEditor::NotifyWarning(NSLOCTEXT("TextureGraphMaterialBridge", "NoSupportedTexturesSelected", "Texture Graph Material Bridge found no supported selected textures."));
		return;
	}

	FString PackagePath;
	UTextureGraph* TextureGraph = UE::TextureGraphMaterialBridgeEditor::CreateTextureGraphAsset(SupportedTextures, PackagePath);
	if (!TextureGraph)
	{
		UE::TextureGraphMaterialBridgeEditor::NotifyWarning(NSLOCTEXT("TextureGraphMaterialBridge", "CreateTextureGraphAssetFailed", "Texture Graph Material Bridge could not create a Texture Graph asset."));
		return;
	}

	TextureGraph->Modify();
	TextureGraph->Graph()->Modify();

	TSet<FName> UsedOutputNames;
	int32 SuccessfulRows = 0;
	for (UTexture* Texture : SupportedTextures)
	{
		const FName OutputName = UE::TextureGraphMaterialBridgeEditor::MakeUniqueOutputName(Texture, UsedOutputNames);
		if (UE::TextureGraphMaterialBridgeEditor::AddTextureOutputRow(TextureGraph, Texture, OutputName, PackagePath, SuccessfulRows, SuccessfulRows == 0))
		{
			++SuccessfulRows;
		}
		else
		{
			UE_LOG(LogTextureGraphMaterialBridgeTextureGraphCreationService, Warning, TEXT("Could not add texture '%s' to generated Texture Graph '%s'."), *Texture->GetPathName(), *TextureGraph->GetPathName());
		}
	}

	if (SuccessfulRows == 0)
	{
		UE::TextureGraphMaterialBridgeEditor::NotifyWarning(NSLOCTEXT("TextureGraphMaterialBridge", "NoTextureGraphRowsCreated", "Texture Graph Material Bridge could not wire any selected textures into the generated Texture Graph."));
		return;
	}

	UE::TextureGraphMaterialBridgeEditor::SaveAndOpenTextureGraph(TextureGraph);
}
