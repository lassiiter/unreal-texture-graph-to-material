#include "TextureGraphMaterialBridgeMaterialInstanceBindingService.h"

#include "TextureGraphMaterialBridgeExpressionUtils.h"
#include "TextureGraphMaterialBridgeMaterialInstanceBindings.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IContentBrowserSingleton.h"
#include "IDetailsView.h"
#include "MaterialEditorModule.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "ScopedTransaction.h"
#include "PropertyEditorDelegates.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "TextureGraph.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "MaterialEditor/DEditorTextureParameterValue.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "Engine/Texture.h"
#include "EditorSupportDelegates.h"
#include "Framework/Notifications/NotificationManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogTextureGraphMaterialBridgeMaterialInstanceBindings, Log, All);

#define LOCTEXT_NAMESPACE "TextureGraphMaterialBridgeMaterialInstanceBindings"

namespace
{
	bool MatchesParameterInfo(const FMaterialParameterInfo& A, const FMaterialParameterInfo& B)
	{
		return A.Name == B.Name && A.Association == B.Association && A.Index == B.Index;
	}

	FString DescribeContextObjects(const TArray<UObject*>& ContextObjects)
	{
		TArray<FString> ObjectDescriptions;
		ObjectDescriptions.Reserve(ContextObjects.Num());
		for (const UObject* ContextObject : ContextObjects)
		{
			ObjectDescriptions.Add(ContextObject
				? FString::Printf(TEXT("%s:%s"), *ContextObject->GetClass()->GetName(), *ContextObject->GetPathName())
				: TEXT("None"));
		}

		return FString::Join(ObjectDescriptions, TEXT(", "));
	}

	UTextureGraphMaterialInstanceBindingsAssetUserData* GetBindingsUserData(UMaterialInstanceConstant* MaterialInstance, bool bCreate)
	{
		if (!MaterialInstance)
		{
			return nullptr;
		}

		UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings =
			Cast<UTextureGraphMaterialInstanceBindingsAssetUserData>(
				MaterialInstance->GetAssetUserDataOfClass(UTextureGraphMaterialInstanceBindingsAssetUserData::StaticClass()));
		if (!Bindings && bCreate)
		{
			MaterialInstance->Modify();
			Bindings = NewObject<UTextureGraphMaterialInstanceBindingsAssetUserData>(MaterialInstance, NAME_None, RF_Transactional);
			MaterialInstance->AddAssetUserData(Bindings);
		}

		return Bindings;
	}

	void ShowBindingNotification(const FText& Message, SNotificationItem::ECompletionState CompletionState)
	{
		FNotificationInfo Info(Message);
		Info.ExpireDuration = 5.0f;
		Info.bUseSuccessFailIcons = true;

		if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info))
		{
			Notification->SetCompletionState(CompletionState);
		}
	}

	void UpdateBindingStatus(
		UMaterialInstanceConstant* MaterialInstance,
		const FMaterialParameterInfo& ParameterInfo,
		ETextureGraphMaterialInstanceBindingStatus Status,
		const FString& StatusMessage)
	{
		if (UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, false))
		{
			if (FTextureGraphMaterialInstanceParameterBinding* Binding = Bindings->FindBinding(ParameterInfo))
			{
				MaterialInstance->Modify();
				Binding->Status = Status;
				Binding->StatusMessage = StatusMessage;
				MaterialInstance->MarkPackageDirty();
			}
		}
	}

	bool ResolveExistingTextureGraphExport(UTextureGraphBase* TextureGraph, FName OutputName, FName& OutCanonicalOutputName, UTexture*& OutTexture, FString& OutError)
	{
		OutCanonicalOutputName = NAME_None;
		OutTexture = nullptr;

		const UTG_Expression_Output* OutputExpression =
			UE::TextureGraphMaterialBridge::FindTextureGraphOutputExpression(TextureGraph, OutputName, &OutError);
		if (!OutputExpression)
		{
			return false;
		}

		OutCanonicalOutputName = UE::TextureGraphMaterialBridge::GetTextureGraphOutputName(*OutputExpression);
		OutTexture = UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraph, OutCanonicalOutputName, &OutError);
		if (!OutTexture)
		{
			return false;
		}

		return true;
	}

	void RefreshMaterialInstanceEditor(UMaterialInstanceConstant* MaterialInstance, UMaterialEditorInstanceConstant* EditorInstance)
	{
		if (EditorInstance)
		{
			EditorInstance->RegenerateArrays();
			if (TSharedPtr<IDetailsView> DetailsView = EditorInstance->DetailsView.Pin())
			{
				DetailsView->ForceRefresh();
			}
		}

		FEditorSupportDelegates::RedrawAllViewports.Broadcast();
	}

	void ApplyExportedTextureToMaterialInstance(
		UMaterialInstanceConstant* MaterialInstance,
		const FMaterialParameterInfo& ParameterInfo,
		UTexture* Texture,
		UDEditorTextureParameterValue* TextureParameter,
		UMaterialEditorInstanceConstant* EditorInstance)
	{
		if (!MaterialInstance || !Texture)
		{
			return;
		}

		MaterialInstance->Modify();
		MaterialInstance->SetTextureParameterValueEditorOnly(ParameterInfo, Texture);
		MaterialInstance->PostEditChange();
		MaterialInstance->MarkPackageDirty();

		if (TextureParameter)
		{
			TextureParameter->Modify();
			TextureParameter->bOverride = true;
			TextureParameter->ParameterValue = Texture;
		}

		RefreshMaterialInstanceEditor(MaterialInstance, EditorInstance);
	}

	void ClearMaterialInstanceTextureOverride(
		UMaterialInstanceConstant* MaterialInstance,
		const FMaterialParameterInfo& ParameterInfo,
		UDEditorTextureParameterValue* TextureParameter,
		UMaterialEditorInstanceConstant* EditorInstance)
	{
		if (!MaterialInstance)
		{
			return;
		}

		MaterialInstance->Modify();
		MaterialInstance->TextureParameterValues.RemoveAll([&ParameterInfo](const FTextureParameterValue& TextureParameterValue)
		{
			return MatchesParameterInfo(TextureParameterValue.ParameterInfo, ParameterInfo);
		});
		MaterialInstance->PostEditChange();
		MaterialInstance->MarkPackageDirty();

		if (TextureParameter)
		{
			TextureParameter->Modify();
			TextureParameter->bOverride = false;
		}

		RefreshMaterialInstanceEditor(MaterialInstance, EditorInstance);
	}

	bool IsStandardTexture2DParameter(UMaterialInstanceConstant* MaterialInstance, const FGuid& ExpressionId)
	{
		if (!MaterialInstance || !ExpressionId.IsValid())
		{
			return false;
		}

		UMaterial* BaseMaterial = MaterialInstance->GetMaterial();
		return BaseMaterial && BaseMaterial->FindExpressionByGUID<UMaterialExpressionTextureSampleParameter2D>(ExpressionId) != nullptr;
	}

	UMaterialInstanceConstant* ResolveSourceMaterialInstance(UMaterialEditorInstanceConstant* EditorInstance)
	{
		return EditorInstance ? Cast<UMaterialInstanceConstant>(EditorInstance->GetMaterialInterface()) : nullptr;
	}

	UDEditorTextureParameterValue* GetTextureParameterFromRowArgs(const FOnGenerateGlobalRowExtensionArgs& Args)
	{
		if (!Args.PropertyHandle.IsValid())
		{
			return nullptr;
		}

		const FProperty* Property = Args.PropertyHandle->GetProperty();
		if (!Property || Property->GetFName() != GET_MEMBER_NAME_CHECKED(UDEditorTextureParameterValue, ParameterValue))
		{
			return nullptr;
		}

		TArray<UObject*> OuterObjects;
		Args.PropertyHandle->GetOuterObjects(OuterObjects);
		if (OuterObjects.Num() != 1)
		{
			return nullptr;
		}

		return Cast<UDEditorTextureParameterValue>(OuterObjects[0]);
	}

	struct FTextureGraphOutputPickerState
	{
		TWeakObjectPtr<UTextureGraphBase> SelectedTextureGraph;
		TArray<TSharedPtr<FString>> OutputOptions;
		TSharedPtr<FString> SelectedOutput;
		bool bAccepted = false;
		TSharedPtr<SComboBox<TSharedPtr<FString>>> OutputComboBox;

		void SetTextureGraph(UTextureGraphBase* TextureGraph)
		{
			SelectedTextureGraph = TextureGraph;
			OutputOptions.Reset();
			SelectedOutput.Reset();

			TArray<FName> OutputNames;
			UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraph, OutputNames);
			for (FName OutputName : OutputNames)
			{
				OutputOptions.Add(MakeShared<FString>(OutputName.ToString()));
			}

			if (OutputOptions.Num() == 1)
			{
				SelectedOutput = OutputOptions[0];
			}

			if (OutputComboBox.IsValid())
			{
				OutputComboBox->RefreshOptions();
				OutputComboBox->SetSelectedItem(SelectedOutput);
			}
		}

		FName GetSelectedOutputName() const
		{
			return SelectedOutput.IsValid() ? FName(**SelectedOutput) : NAME_None;
		}
	};

	bool PickTextureGraphOutput(UTextureGraphBase*& OutTextureGraph, FName& OutOutputName)
	{
		OutTextureGraph = nullptr;
		OutOutputName = NAME_None;

		TSharedRef<FTextureGraphOutputPickerState> State = MakeShared<FTextureGraphOutputPickerState>();

		FAssetPickerConfig AssetPickerConfig;
		AssetPickerConfig.SelectionMode = ESelectionMode::Single;
		AssetPickerConfig.Filter.ClassPaths.Add(UTextureGraph::StaticClass()->GetClassPathName());
		AssetPickerConfig.Filter.ClassPaths.Add(UTextureGraphInstance::StaticClass()->GetClassPathName());
		AssetPickerConfig.Filter.bRecursiveClasses = true;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::Column;
		AssetPickerConfig.bAllowDragging = false;
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.bShowBottomToolbar = false;
		AssetPickerConfig.bCanShowClasses = false;
		AssetPickerConfig.bAddFilterUI = true;
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateLambda([State](const FAssetData& AssetData)
		{
			State->SetTextureGraph(Cast<UTextureGraphBase>(AssetData.GetAsset()));
		});

		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

		TSharedPtr<SWindow> PickerWindow;
		PickerWindow = SNew(SWindow)
			.Title(LOCTEXT("PickTextureGraphOutputTitle", "Bind Texture Graph Output"))
			.ClientSize(FVector2D(760.0f, 560.0f))
			.SupportsMaximize(false)
			.SupportsMinimize(false);
		TWeakPtr<SWindow> WeakPickerWindow = PickerWindow;

		PickerWindow->SetContent(
			SNew(SBorder)
			.Padding(10.0f)
			.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 10.0f, 0.0f, 0.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0.0f, 0.0f, 8.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("OutputLabel", "Output"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SAssignNew(State->OutputComboBox, SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&State->OutputOptions)
						.OnSelectionChanged_Lambda([State](TSharedPtr<FString> NewSelection, ESelectInfo::Type)
						{
							State->SelectedOutput = NewSelection;
						})
						.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
						{
							return SNew(STextBlock).Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty());
						})
						[
							SNew(STextBlock)
							.Text_Lambda([State]()
							{
								if (State->SelectedOutput.IsValid())
								{
									return FText::FromString(*State->SelectedOutput);
								}

								return State->SelectedTextureGraph.IsValid()
									? LOCTEXT("NoOutputsSelected", "No output selected")
									: LOCTEXT("NoGraphSelected", "Select a Texture Graph");
							})
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 6.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.ColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.45f, 0.2f)))
					.Text_Lambda([State]()
					{
						if (State->SelectedTextureGraph.IsValid() && State->OutputOptions.IsEmpty())
						{
							return LOCTEXT("TextureGraphHasNoOutputs", "This Texture Graph has no outputs.");
						}

						return FText::GetEmpty();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(0.0f, 12.0f, 0.0f, 0.0f)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FMargin(4.0f, 0.0f))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("BindButton", "Bind"))
						.IsEnabled_Lambda([State]()
						{
							return State->SelectedTextureGraph.IsValid() && State->SelectedOutput.IsValid();
						})
						.OnClicked_Lambda([State, WeakPickerWindow]()
						{
							State->bAccepted = true;
							if (TSharedPtr<SWindow> PinnedPickerWindow = WeakPickerWindow.Pin())
							{
								PinnedPickerWindow->RequestDestroyWindow();
							}
							return FReply::Handled();
						})
					]
					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("CancelButton", "Cancel"))
						.OnClicked_Lambda([WeakPickerWindow]()
						{
							if (TSharedPtr<SWindow> PinnedPickerWindow = WeakPickerWindow.Pin())
							{
								PinnedPickerWindow->RequestDestroyWindow();
							}
							return FReply::Handled();
						})
					]
				]
			]);

		FSlateApplication::Get().AddModalWindow(PickerWindow.ToSharedRef(), nullptr);

		if (!State->bAccepted)
		{
			return false;
		}

		OutTextureGraph = State->SelectedTextureGraph.Get();
		OutOutputName = State->GetSelectedOutputName();
		return OutTextureGraph && !OutOutputName.IsNone();
	}

	struct FTextureParameterPickerOption
	{
		FMaterialParameterInfo ParameterInfo;
		FString Label;
	};

	struct FMaterialInstanceBindingPickerState : public FTextureGraphOutputPickerState
	{
		TArray<TSharedPtr<FTextureParameterPickerOption>> ParameterOptions;
		TSharedPtr<FTextureParameterPickerOption> SelectedParameter;
	};

	bool PickMaterialInstanceBinding(
		UMaterialInstanceConstant* MaterialInstance,
		const TArray<FMaterialParameterInfo>& EligibleParameters,
		FMaterialParameterInfo& OutParameterInfo,
		UTextureGraphBase*& OutTextureGraph,
		FName& OutOutputName)
	{
		OutParameterInfo = FMaterialParameterInfo();
		OutTextureGraph = nullptr;
		OutOutputName = NAME_None;

		if (!MaterialInstance || EligibleParameters.IsEmpty())
		{
			return false;
		}

		TSharedRef<FMaterialInstanceBindingPickerState> State = MakeShared<FMaterialInstanceBindingPickerState>();
		for (const FMaterialParameterInfo& ParameterInfo : EligibleParameters)
		{
			TSharedRef<FTextureParameterPickerOption> Option = MakeShared<FTextureParameterPickerOption>();
			Option->ParameterInfo = ParameterInfo;
			Option->Label = ParameterInfo.Name.ToString();
			if (ParameterInfo.Association != EMaterialParameterAssociation::GlobalParameter)
			{
				Option->Label += FString::Printf(TEXT(" [%d]"), ParameterInfo.Index);
			}
			State->ParameterOptions.Add(Option);
		}
		State->SelectedParameter = State->ParameterOptions[0];

		FAssetPickerConfig AssetPickerConfig;
		AssetPickerConfig.SelectionMode = ESelectionMode::Single;
		AssetPickerConfig.Filter.ClassPaths.Add(UTextureGraph::StaticClass()->GetClassPathName());
		AssetPickerConfig.Filter.ClassPaths.Add(UTextureGraphInstance::StaticClass()->GetClassPathName());
		AssetPickerConfig.Filter.bRecursiveClasses = true;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::Column;
		AssetPickerConfig.bAllowDragging = false;
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.bShowBottomToolbar = false;
		AssetPickerConfig.bCanShowClasses = false;
		AssetPickerConfig.bAddFilterUI = true;
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateLambda([State](const FAssetData& AssetData)
		{
			State->SetTextureGraph(Cast<UTextureGraphBase>(AssetData.GetAsset()));
		});

		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

		TSharedPtr<SWindow> PickerWindow;
		PickerWindow = SNew(SWindow)
			.Title(LOCTEXT("PickMaterialInstanceBindingTitle", "Bind Texture Graph Output"))
			.ClientSize(FVector2D(760.0f, 600.0f))
			.SupportsMaximize(false)
			.SupportsMinimize(false);
		TWeakPtr<SWindow> WeakPickerWindow = PickerWindow;

		PickerWindow->SetContent(
			SNew(SBorder)
			.Padding(10.0f)
			.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0.0f, 0.0f, 8.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("TextureParameterLabel", "Parameter"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SComboBox<TSharedPtr<FTextureParameterPickerOption>>)
						.OptionsSource(&State->ParameterOptions)
						.InitiallySelectedItem(State->SelectedParameter)
						.OnSelectionChanged_Lambda([State](TSharedPtr<FTextureParameterPickerOption> NewSelection, ESelectInfo::Type)
						{
							State->SelectedParameter = NewSelection;
						})
						.OnGenerateWidget_Lambda([](TSharedPtr<FTextureParameterPickerOption> Item)
						{
							return SNew(STextBlock).Text(Item.IsValid() ? FText::FromString(Item->Label) : FText::GetEmpty());
						})
						[
							SNew(STextBlock)
							.Text_Lambda([State]()
							{
								return State->SelectedParameter.IsValid()
									? FText::FromString(State->SelectedParameter->Label)
									: LOCTEXT("NoParameterSelected", "Select a parameter");
							})
						]
					]
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 10.0f, 0.0f, 0.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0.0f, 0.0f, 8.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("OutputLabelFromMaterialInstanceDialog", "Output"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SAssignNew(State->OutputComboBox, SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&State->OutputOptions)
						.OnSelectionChanged_Lambda([State](TSharedPtr<FString> NewSelection, ESelectInfo::Type)
						{
							State->SelectedOutput = NewSelection;
						})
						.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
						{
							return SNew(STextBlock).Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty());
						})
						[
							SNew(STextBlock)
							.Text_Lambda([State]()
							{
								if (State->SelectedOutput.IsValid())
								{
									return FText::FromString(*State->SelectedOutput);
								}

								return State->SelectedTextureGraph.IsValid()
									? LOCTEXT("NoOutputSelectedFromMaterialInstanceDialog", "No output selected")
									: LOCTEXT("NoGraphSelectedFromMaterialInstanceDialog", "Select a Texture Graph");
							})
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 6.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.ColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.45f, 0.2f)))
					.Text_Lambda([State]()
					{
						if (State->SelectedTextureGraph.IsValid() && State->OutputOptions.IsEmpty())
						{
							return LOCTEXT("SelectedTextureGraphHasNoOutputs", "This Texture Graph has no outputs.");
						}

						return FText::GetEmpty();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(0.0f, 12.0f, 0.0f, 0.0f)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FMargin(4.0f, 0.0f))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("BindMaterialInstanceButton", "Bind"))
						.IsEnabled_Lambda([State]()
						{
							return State->SelectedParameter.IsValid()
								&& State->SelectedTextureGraph.IsValid()
								&& State->SelectedOutput.IsValid();
						})
						.OnClicked_Lambda([State, WeakPickerWindow]()
						{
							State->bAccepted = true;
							if (TSharedPtr<SWindow> PinnedPickerWindow = WeakPickerWindow.Pin())
							{
								PinnedPickerWindow->RequestDestroyWindow();
							}
							return FReply::Handled();
						})
					]
					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("CancelMaterialInstanceBindingButton", "Cancel"))
						.OnClicked_Lambda([WeakPickerWindow]()
						{
							if (TSharedPtr<SWindow> PinnedPickerWindow = WeakPickerWindow.Pin())
							{
								PinnedPickerWindow->RequestDestroyWindow();
							}
							return FReply::Handled();
						})
					]
				]
			]);

		FSlateApplication::Get().AddModalWindow(PickerWindow.ToSharedRef(), nullptr);

		if (!State->bAccepted || !State->SelectedParameter.IsValid())
		{
			return false;
		}

		OutParameterInfo = State->SelectedParameter->ParameterInfo;
		OutTextureGraph = State->SelectedTextureGraph.Get();
		OutOutputName = State->GetSelectedOutputName();
		return OutTextureGraph && !OutOutputName.IsNone();
	}
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::Startup()
{
	UE_LOG(LogTextureGraphMaterialBridgeMaterialInstanceBindings, Log, TEXT("Starting Material Instance Texture Graph binding service."));

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	RowExtensionHandle = PropertyEditorModule.GetGlobalRowExtensionDelegate().AddRaw(
		this,
		&FTextureGraphMaterialBridgeMaterialInstanceBindingService::HandleGenerateGlobalRowExtension);
	UE_LOG(LogTextureGraphMaterialBridgeMaterialInstanceBindings, Log, TEXT("Registered global row extension delegate for Texture Graph Material Instance bindings."));

	IMaterialEditorModule& MaterialEditorModule = FModuleManager::LoadModuleChecked<IMaterialEditorModule>("MaterialEditor");
	FAssetEditorExtender ToolbarExtender = FAssetEditorExtender::CreateRaw(
		this,
		&FTextureGraphMaterialBridgeMaterialInstanceBindingService::CreateMaterialInstanceToolbarExtender);
	MaterialEditorToolbarExtenderHandle = ToolbarExtender.GetHandle();
	MaterialEditorModule.GetToolBarExtensibilityManager()->GetExtenderDelegates().Add(ToolbarExtender);
	UE_LOG(LogTextureGraphMaterialBridgeMaterialInstanceBindings, Log, TEXT("Registered Material Editor toolbar extender for Texture Graph Material Instance bindings."));
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::Shutdown()
{
	UE_LOG(LogTextureGraphMaterialBridgeMaterialInstanceBindings, Log, TEXT("Shutting down Material Instance Texture Graph binding service."));

	if (RowExtensionHandle.IsValid())
	{
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyEditorModule->GetGlobalRowExtensionDelegate().Remove(RowExtensionHandle);
		}

		RowExtensionHandle.Reset();
	}

	if (MaterialEditorToolbarExtenderHandle.IsValid())
	{
		if (IMaterialEditorModule* MaterialEditorModule = FModuleManager::GetModulePtr<IMaterialEditorModule>("MaterialEditor"))
		{
			TArray<FAssetEditorExtender>& ExtenderDelegates = MaterialEditorModule->GetToolBarExtensibilityManager()->GetExtenderDelegates();
			ExtenderDelegates.RemoveAll([this](const FAssetEditorExtender& ExtenderDelegate)
			{
				return ExtenderDelegate.GetHandle() == MaterialEditorToolbarExtenderHandle;
			});
		}

		MaterialEditorToolbarExtenderHandle.Reset();
	}
}

TSharedRef<FExtender> FTextureGraphMaterialBridgeMaterialInstanceBindingService::CreateMaterialInstanceToolbarExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextObjects)
{
	TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();

	UE_LOG(
		LogTextureGraphMaterialBridgeMaterialInstanceBindings,
		Log,
		TEXT("Material Instance toolbar extender invoked with %d context object(s): %s"),
		ContextObjects.Num(),
		*DescribeContextObjects(ContextObjects));

	UMaterialInstanceConstant* MaterialInstance = nullptr;
	for (UObject* ContextObject : ContextObjects)
	{
		MaterialInstance = Cast<UMaterialInstanceConstant>(ContextObject);
		if (MaterialInstance)
		{
			break;
		}
	}

	if (!MaterialInstance)
	{
		UE_LOG(
			LogTextureGraphMaterialBridgeMaterialInstanceBindings,
			Log,
			TEXT("Texture Graph toolbar extension skipped because no UMaterialInstanceConstant was present in the toolbar context."));
		return ToolbarExtender;
	}

	UE_LOG(
		LogTextureGraphMaterialBridgeMaterialInstanceBindings,
		Log,
		TEXT("Adding Texture Graph toolbar extension for Material Instance '%s'."),
		*MaterialInstance->GetPathName());

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		CommandList,
		FToolBarExtensionDelegate::CreateRaw(
			this,
			&FTextureGraphMaterialBridgeMaterialInstanceBindingService::FillMaterialInstanceToolbar,
			TWeakObjectPtr<UMaterialInstanceConstant>(MaterialInstance)));

	return ToolbarExtender;
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::FillMaterialInstanceToolbar(FToolBarBuilder& ToolbarBuilder, TWeakObjectPtr<UMaterialInstanceConstant> MaterialInstance) const
{
	UE_LOG(
		LogTextureGraphMaterialBridgeMaterialInstanceBindings,
		Log,
		TEXT("Filling Texture Graph Material Instance toolbar button for '%s'."),
		MaterialInstance.IsValid() ? *MaterialInstance->GetPathName() : TEXT("None"));

	ToolbarBuilder.AddToolBarButton(
		FUIAction(
			FExecuteAction::CreateLambda([this, MaterialInstance]()
			{
				OpenBindingDialog(MaterialInstance.Get());
			}),
			FCanExecuteAction::CreateLambda([MaterialInstance]()
			{
				return MaterialInstance.IsValid();
			})),
		NAME_None,
		LOCTEXT("BindTextureGraphToolbarLabel", "Texture Graph"),
		LOCTEXT("BindTextureGraphToolbarToolTip", "Bind a Material Instance texture parameter to a Texture Graph output."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.TextureGraph"));
}

TArray<FMaterialParameterInfo> FTextureGraphMaterialBridgeMaterialInstanceBindingService::ListEligibleTextureParameters(UMaterialInstanceConstant* MaterialInstance) const
{
	TArray<FMaterialParameterInfo> EligibleParameters;
	if (!MaterialInstance)
	{
		return EligibleParameters;
	}

	TArray<FMaterialParameterInfo> ParameterInfos;
	TArray<FGuid> ParameterIds;
	MaterialInstance->GetAllTextureParameterInfo(ParameterInfos, ParameterIds);

	for (int32 Index = 0; Index < ParameterInfos.Num(); ++Index)
	{
		const FGuid ExpressionId = ParameterIds.IsValidIndex(Index) ? ParameterIds[Index] : FGuid();
		if (IsStandardTexture2DParameter(MaterialInstance, ExpressionId))
		{
			EligibleParameters.Add(ParameterInfos[Index]);
		}
	}

	return EligibleParameters;
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::OpenBindingDialog(UMaterialInstanceConstant* MaterialInstance) const
{
	if (!MaterialInstance)
	{
		UE_LOG(LogTextureGraphMaterialBridgeMaterialInstanceBindings, Warning, TEXT("Texture Graph binding dialog was requested without a Material Instance."));
		return;
	}

	TArray<FMaterialParameterInfo> EligibleParameters = ListEligibleTextureParameters(MaterialInstance);
	UE_LOG(
		LogTextureGraphMaterialBridgeMaterialInstanceBindings,
		Log,
		TEXT("Opening Texture Graph binding dialog for '%s'. Eligible standard TextureSampleParameter2D count: %d."),
		*MaterialInstance->GetPathName(),
		EligibleParameters.Num());
	if (EligibleParameters.IsEmpty())
	{
		ShowBindingNotification(
			FText::Format(LOCTEXT("NoEligibleTextureParameters", "{0} has no standard TextureSampleParameter2D parameters to bind."), FText::FromString(MaterialInstance->GetName())),
			SNotificationItem::CS_Fail);
		return;
	}

	FMaterialParameterInfo ParameterInfo;
	UTextureGraphBase* TextureGraph = nullptr;
	FName OutputName = NAME_None;
	if (!PickMaterialInstanceBinding(MaterialInstance, EligibleParameters, ParameterInfo, TextureGraph, OutputName))
	{
		return;
	}

	BindTextureGraphOutput(MaterialInstance, ParameterInfo, TextureGraph, OutputName, nullptr, nullptr);
}

TArray<FSoftObjectPath> FTextureGraphMaterialBridgeMaterialInstanceBindingService::FindBoundMaterialInstancePaths(const UTextureGraphBase* TextureGraph) const
{
	TArray<FSoftObjectPath> MaterialInstancePaths;
	if (!TextureGraph)
	{
		return MaterialInstancePaths;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FName> ReferencerPackages;
	AssetRegistry.GetReferencers(
		TextureGraph->GetOutermost()->GetFName(),
		ReferencerPackages,
		UE::AssetRegistry::EDependencyCategory::Package,
		UE::AssetRegistry::EDependencyQuery::Hard | UE::AssetRegistry::EDependencyQuery::Soft);

	for (const FName ReferencerPackage : ReferencerPackages)
	{
		TArray<FAssetData> AssetsInPackage;
		if (!AssetRegistry.GetAssetsByPackageName(ReferencerPackage, AssetsInPackage))
		{
			continue;
		}

		for (const FAssetData& AssetData : AssetsInPackage)
		{
			if (!AssetData.IsInstanceOf(UMaterialInstanceConstant::StaticClass()))
			{
				continue;
			}

			UMaterialInstanceConstant* MaterialInstance = Cast<UMaterialInstanceConstant>(AssetData.GetAsset());
			const UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, false);
			if (Bindings && Bindings->ReferencesTextureGraph(TextureGraph))
			{
				MaterialInstancePaths.AddUnique(AssetData.ToSoftObjectPath());
			}
		}
	}

	return MaterialInstancePaths;
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::RefreshBindingsForTextureGraph(UTextureGraphBase* TextureGraph, const TArray<FSoftObjectPath>& MaterialInstancePaths) const
{
	if (!TextureGraph)
	{
		return;
	}

	for (const FSoftObjectPath& MaterialInstancePath : MaterialInstancePaths)
	{
		UMaterialInstanceConstant* MaterialInstance = Cast<UMaterialInstanceConstant>(MaterialInstancePath.TryLoad());
		UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, false);
		if (!Bindings)
		{
			continue;
		}

		TArray<FTextureGraphMaterialInstanceParameterBinding> BindingsToRebuild;
		for (const FTextureGraphMaterialInstanceParameterBinding& Binding : Bindings->Bindings)
		{
			if (Binding.bEnabled && Binding.ReferencesTextureGraph(TextureGraph))
			{
				BindingsToRebuild.Add(Binding);
			}
		}

		for (const FTextureGraphMaterialInstanceParameterBinding& Binding : BindingsToRebuild)
		{
			RefreshBinding(MaterialInstance, Binding);
		}
	}
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::HandleGenerateGlobalRowExtension(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensions)
{
	UDEditorTextureParameterValue* TextureParameter = GetTextureParameterFromRowArgs(Args);
	UMaterialEditorInstanceConstant* EditorInstance = TextureParameter ? Cast<UMaterialEditorInstanceConstant>(TextureParameter->GetOuter()) : nullptr;
	UMaterialInstanceConstant* MaterialInstance = ResolveSourceMaterialInstance(EditorInstance);
	if (!TextureParameter || !EditorInstance || !IsStandardTexture2DParameter(MaterialInstance, TextureParameter->ExpressionId))
	{
		return;
	}

	TWeakObjectPtr<UDEditorTextureParameterValue> WeakTextureParameter(TextureParameter);
	TWeakObjectPtr<UMaterialEditorInstanceConstant> WeakEditorInstance(EditorInstance);
	TWeakObjectPtr<UMaterialInstanceConstant> WeakMaterialInstance(MaterialInstance);

	const UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, false);
	const FTextureGraphMaterialInstanceParameterBinding* ExistingBinding = Bindings ? Bindings->FindBinding(TextureParameter->ParameterInfo) : nullptr;

	FPropertyRowExtensionButton BindButton;
	BindButton.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), ExistingBinding ? "Icons.Edit" : "Icons.Link");
	BindButton.Label = ExistingBinding ? LOCTEXT("EditTextureGraphBindingLabel", "Edit Texture Graph Binding") : LOCTEXT("BindTextureGraphLabel", "Bind Texture Graph");
	BindButton.ToolTip = ExistingBinding
		? FText::Format(
			LOCTEXT("EditTextureGraphBindingTooltip", "Bound to {0} / {1}. Click to choose another Texture Graph output."),
			FText::FromString(ExistingBinding->TextureGraphPath.ToString()),
			FText::FromName(ExistingBinding->OutputName))
		: LOCTEXT("BindTextureGraphTooltip", "Bind this texture parameter to a Texture Graph output.");
	BindButton.UIAction = FUIAction(FExecuteAction::CreateLambda([this, WeakTextureParameter, WeakEditorInstance]()
	{
		BindFromPicker(WeakTextureParameter.Get(), WeakEditorInstance.Get());
	}));
	OutExtensions.Add(BindButton);

	if (!ExistingBinding)
	{
		return;
	}

	FTextureGraphMaterialInstanceParameterBinding BindingCopy = *ExistingBinding;

	FPropertyRowExtensionButton RefreshButton;
	RefreshButton.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Refresh");
	RefreshButton.Label = LOCTEXT("RefreshTextureGraphBindingLabel", "Refresh Texture Graph Binding");
	RefreshButton.ToolTip = LOCTEXT("RefreshTextureGraphBindingTooltip", "Resolve the Texture Graph output's existing exported texture and update this material instance parameter.");
	RefreshButton.UIAction = FUIAction(FExecuteAction::CreateLambda([this, WeakMaterialInstance, BindingCopy, WeakTextureParameter, WeakEditorInstance]()
	{
		RefreshBinding(WeakMaterialInstance.Get(), BindingCopy, WeakTextureParameter.Get(), WeakEditorInstance.Get());
	}));
	OutExtensions.Add(RefreshButton);

	FPropertyRowExtensionButton ClearButton;
	ClearButton.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Delete");
	ClearButton.Label = LOCTEXT("ClearTextureGraphBindingLabel", "Clear Texture Graph Binding");
	ClearButton.ToolTip = LOCTEXT("ClearTextureGraphBindingTooltip", "Remove the Texture Graph binding and clear the material instance texture override.");
	ClearButton.UIAction = FUIAction(FExecuteAction::CreateLambda([this, WeakMaterialInstance, ParameterInfo = TextureParameter->ParameterInfo, WeakTextureParameter, WeakEditorInstance]()
	{
		ClearBinding(WeakMaterialInstance.Get(), ParameterInfo, WeakTextureParameter.Get(), WeakEditorInstance.Get());
	}));
	OutExtensions.Add(ClearButton);
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::BindFromPicker(UDEditorTextureParameterValue* TextureParameter, UMaterialEditorInstanceConstant* EditorInstance) const
{
	UMaterialInstanceConstant* MaterialInstance = ResolveSourceMaterialInstance(EditorInstance);
	if (!TextureParameter || !MaterialInstance)
	{
		return;
	}

	UTextureGraphBase* TextureGraph = nullptr;
	FName OutputName = NAME_None;
	if (!PickTextureGraphOutput(TextureGraph, OutputName))
	{
		return;
	}

	BindTextureGraphOutput(MaterialInstance, TextureParameter->ParameterInfo, TextureGraph, OutputName, TextureParameter, EditorInstance);
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::BindTextureGraphOutput(
	UMaterialInstanceConstant* MaterialInstance,
	const FMaterialParameterInfo& ParameterInfo,
	UTextureGraphBase* TextureGraph,
	FName OutputName,
	UDEditorTextureParameterValue* TextureParameter,
	UMaterialEditorInstanceConstant* EditorInstance) const
{
	if (!MaterialInstance || !TextureGraph || OutputName.IsNone())
	{
		return;
	}

	FName CanonicalOutputName = NAME_None;
	UTexture* ExportedTexture = nullptr;
	FString ErrorMessage;
	if (!ResolveExistingTextureGraphExport(TextureGraph, OutputName, CanonicalOutputName, ExportedTexture, ErrorMessage))
	{
		ShowBindingNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail);
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("BindTextureGraphOutputTransaction", "Bind Texture Graph Output To Material Instance"));

	FTextureGraphMaterialInstanceParameterBinding Binding;
	Binding.ParameterInfo = ParameterInfo;
	Binding.TextureGraphPath = FSoftObjectPath(TextureGraph);
	Binding.OutputName = CanonicalOutputName;
	Binding.ExportedTexturePath = FSoftObjectPath(ExportedTexture);
	Binding.bEnabled = true;
	Binding.Status = ETextureGraphMaterialInstanceBindingStatus::Ready;
	Binding.StatusMessage = FString::Printf(TEXT("Assigned existing exported texture '%s'."), *ExportedTexture->GetPathName());

	UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, true);
	if (!Bindings)
	{
		return;
	}

	Bindings->AddOrUpdateBinding(Binding);
	MaterialInstance->MarkPackageDirty();

	ExportedTexture->UpdateResource();
	ApplyExportedTextureToMaterialInstance(MaterialInstance, ParameterInfo, ExportedTexture, TextureParameter, EditorInstance);
	ShowBindingNotification(
		FText::Format(
			LOCTEXT("TextureGraphBindingReadyNotification", "Assigned {0} to {1}."),
			FText::FromString(ExportedTexture->GetName()),
			FText::FromName(ParameterInfo.Name)),
		SNotificationItem::CS_Success);
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::RefreshBinding(
	UMaterialInstanceConstant* MaterialInstance,
	const FTextureGraphMaterialInstanceParameterBinding& Binding,
	UDEditorTextureParameterValue* TextureParameter,
	UMaterialEditorInstanceConstant* EditorInstance) const
{
	if (!MaterialInstance || !Binding.bEnabled)
	{
		return;
	}

	UTextureGraphBase* TextureGraph = Cast<UTextureGraphBase>(Binding.TextureGraphPath.TryLoad());
	if (!TextureGraph)
	{
		const FString ErrorMessage = FString::Printf(TEXT("Could not load Texture Graph '%s'."), *Binding.TextureGraphPath.ToString());
		UpdateBindingStatus(MaterialInstance, Binding.ParameterInfo, ETextureGraphMaterialInstanceBindingStatus::Error, ErrorMessage);
		ShowBindingNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail);
		return;
	}

	FName CanonicalOutputName = NAME_None;
	UTexture* ExportedTexture = nullptr;
	FString ErrorMessage;
	if (!ResolveExistingTextureGraphExport(TextureGraph, Binding.OutputName, CanonicalOutputName, ExportedTexture, ErrorMessage))
	{
		UpdateBindingStatus(MaterialInstance, Binding.ParameterInfo, ETextureGraphMaterialInstanceBindingStatus::Error, ErrorMessage);
		ShowBindingNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail);
		return;
	}

	FTextureGraphMaterialInstanceParameterBinding UpdatedBinding = Binding;
	UpdatedBinding.OutputName = CanonicalOutputName;
	UpdatedBinding.ExportedTexturePath = FSoftObjectPath(ExportedTexture);
	UpdatedBinding.Status = ETextureGraphMaterialInstanceBindingStatus::Ready;
	UpdatedBinding.StatusMessage = FString::Printf(TEXT("Assigned existing exported texture '%s'."), *ExportedTexture->GetPathName());
	if (UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, true))
	{
		MaterialInstance->Modify();
		Bindings->AddOrUpdateBinding(UpdatedBinding);
		MaterialInstance->MarkPackageDirty();
	}

	UE_LOG(
		LogTextureGraphMaterialBridgeMaterialInstanceBindings,
		Log,
		TEXT("Refreshing Material Instance '%s' parameter '%s' from Texture Graph '%s' output '%s' existing exported texture '%s'."),
		*MaterialInstance->GetPathName(),
		*Binding.ParameterInfo.Name.ToString(),
		*TextureGraph->GetPathName(),
		*CanonicalOutputName.ToString(),
		*ExportedTexture->GetPathName());

	ExportedTexture->UpdateResource();
	ApplyExportedTextureToMaterialInstance(MaterialInstance, UpdatedBinding.ParameterInfo, ExportedTexture, TextureParameter, EditorInstance);

	ShowBindingNotification(
		FText::Format(
			LOCTEXT("TextureGraphBindingRefreshedNotification", "Assigned {0} to {1}."),
			FText::FromString(ExportedTexture->GetName()),
			FText::FromName(UpdatedBinding.ParameterInfo.Name)),
		SNotificationItem::CS_Success);
}

void FTextureGraphMaterialBridgeMaterialInstanceBindingService::ClearBinding(
	UMaterialInstanceConstant* MaterialInstance,
	const FMaterialParameterInfo& ParameterInfo,
	UDEditorTextureParameterValue* TextureParameter,
	UMaterialEditorInstanceConstant* EditorInstance) const
{
	if (!MaterialInstance)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("ClearTextureGraphOutputBindingTransaction", "Clear Texture Graph Material Instance Binding"));

	bool bRemovedBinding = false;
	if (UTextureGraphMaterialInstanceBindingsAssetUserData* Bindings = GetBindingsUserData(MaterialInstance, false))
	{
		MaterialInstance->Modify();
		bRemovedBinding = Bindings->RemoveBinding(ParameterInfo);
		if (Bindings->IsEmpty())
		{
			MaterialInstance->RemoveUserDataOfClass(UTextureGraphMaterialInstanceBindingsAssetUserData::StaticClass());
		}
		MaterialInstance->MarkPackageDirty();
	}

	ClearMaterialInstanceTextureOverride(MaterialInstance, ParameterInfo, TextureParameter, EditorInstance);

	if (bRemovedBinding)
	{
		ShowBindingNotification(
			FText::Format(LOCTEXT("TextureGraphBindingClearedNotification", "Cleared Texture Graph binding for {0}."), FText::FromName(ParameterInfo.Name)),
			SNotificationItem::CS_Success);
	}
}

#undef LOCTEXT_NAMESPACE
