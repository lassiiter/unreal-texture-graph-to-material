#include "TextureGraphMaterialBridgeMaterialCreationService.h"

#include "TextureGraphMaterialBridgeEditorExportUtils.h"
#include "MaterialExpressionTextureGraphSample.h"
#include "TextureGraphMaterialBridgeExpressionUtils.h"
#include "Async/Async.h"
#include "Factories/MaterialFactoryNew.h"
#include "FileHelpers.h"
#include "Framework/Notifications/NotificationManager.h"
#include "IAssetTools.h"
#include "MaterialEditingLibrary.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Misc/PackageName.h"
#include "Model/Mix/MixSettings.h"
#include "Model/Mix/ViewportSettings.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TG_HelperFunctions.h"
#include "TextureGraph.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Editor.h"

DEFINE_LOG_CATEGORY_STATIC(LogTextureGraphMaterialBridgeMaterialCreationService, Log, All);

namespace UE::TextureGraphMaterialBridgeEditor
{
	enum class ETextureGraphLinkedMaterialRole : uint8
	{
		BaseColor,
		Normal,
		EmissiveColor,
		AmbientOcclusion,
		Roughness,
		Metallic
	};

	uint32 GetTypeHash(ETextureGraphLinkedMaterialRole Role)
	{
		return static_cast<uint8>(Role);
	}

	struct FTextureGraphMaterialBinding
	{
		FName OutputName = NAME_None;
		FName OutputChannel = NAME_None;
		bool bUsePackedOrmChannels = false;
	};

	struct FTextureGraphMaterialPlan
	{
		TMap<ETextureGraphLinkedMaterialRole, FTextureGraphMaterialBinding> Bindings;
	};

	struct FResolvedTextureGraphMaterialBinding
	{
		ETextureGraphLinkedMaterialRole Role = ETextureGraphLinkedMaterialRole::BaseColor;
		FTextureGraphMaterialBinding Binding;
	};

	const TArray<ETextureGraphLinkedMaterialRole>& GetOrderedMaterialRoles()
	{
		static const TArray<ETextureGraphLinkedMaterialRole> OrderedRoles =
		{
			ETextureGraphLinkedMaterialRole::BaseColor,
			ETextureGraphLinkedMaterialRole::Normal,
			ETextureGraphLinkedMaterialRole::EmissiveColor,
			ETextureGraphLinkedMaterialRole::AmbientOcclusion,
			ETextureGraphLinkedMaterialRole::Roughness,
			ETextureGraphLinkedMaterialRole::Metallic
		};

		return OrderedRoles;
	}

	bool IsPackedOrmRole(ETextureGraphLinkedMaterialRole Role)
	{
		return
			Role == ETextureGraphLinkedMaterialRole::AmbientOcclusion ||
			Role == ETextureGraphLinkedMaterialRole::Roughness ||
			Role == ETextureGraphLinkedMaterialRole::Metallic;
	}

	FString NormalizeAlias(const FString& Value)
	{
		FString Normalized = Value;
		Normalized.ToLowerInline();
		Normalized.ReplaceInline(TEXT(" "), TEXT(""));
		Normalized.ReplaceInline(TEXT("_"), TEXT(""));
		Normalized.ReplaceInline(TEXT("-"), TEXT(""));
		return Normalized;
	}

	bool IsPackedOrmAlias(const FString& NormalizedAlias)
	{
		return NormalizedAlias == TEXT("orm");
	}

	bool TryGetRoleFromAlias(const FString& NormalizedAlias, ETextureGraphLinkedMaterialRole& OutRole)
	{
		if (NormalizedAlias == TEXT("basecolor") || NormalizedAlias == TEXT("albedo") || NormalizedAlias == TEXT("diffuse"))
		{
			OutRole = ETextureGraphLinkedMaterialRole::BaseColor;
			return true;
		}

		if (NormalizedAlias == TEXT("normal") || NormalizedAlias == TEXT("normalmap"))
		{
			OutRole = ETextureGraphLinkedMaterialRole::Normal;
			return true;
		}

		if (NormalizedAlias == TEXT("emissive") || NormalizedAlias == TEXT("emissivecolor"))
		{
			OutRole = ETextureGraphLinkedMaterialRole::EmissiveColor;
			return true;
		}

		if (NormalizedAlias == TEXT("ambientocclusion") || NormalizedAlias == TEXT("occlusion") || NormalizedAlias == TEXT("ao"))
		{
			OutRole = ETextureGraphLinkedMaterialRole::AmbientOcclusion;
			return true;
		}

		if (NormalizedAlias == TEXT("roughness"))
		{
			OutRole = ETextureGraphLinkedMaterialRole::Roughness;
			return true;
		}

		if (NormalizedAlias == TEXT("metallic") || NormalizedAlias == TEXT("metalness"))
		{
			OutRole = ETextureGraphLinkedMaterialRole::Metallic;
			return true;
		}

		return false;
	}

	EMaterialProperty GetMaterialPropertyForRole(ETextureGraphLinkedMaterialRole Role)
	{
		switch (Role)
		{
		case ETextureGraphLinkedMaterialRole::BaseColor:
			return MP_BaseColor;

		case ETextureGraphLinkedMaterialRole::Normal:
			return MP_Normal;

		case ETextureGraphLinkedMaterialRole::EmissiveColor:
			return MP_EmissiveColor;

		case ETextureGraphLinkedMaterialRole::AmbientOcclusion:
			return MP_AmbientOcclusion;

		case ETextureGraphLinkedMaterialRole::Roughness:
			return MP_Roughness;

		case ETextureGraphLinkedMaterialRole::Metallic:
			return MP_Metallic;
		}

		return MP_BaseColor;
	}

	FName GetDefaultOutputChannelForRole(ETextureGraphLinkedMaterialRole Role)
	{
		return IsPackedOrmRole(Role) ? FName(TEXT("R")) : NAME_None;
	}

	FName GetPackedOrmOutputChannelForRole(ETextureGraphLinkedMaterialRole Role)
	{
		switch (Role)
		{
		case ETextureGraphLinkedMaterialRole::AmbientOcclusion:
			return FName(TEXT("R"));

		case ETextureGraphLinkedMaterialRole::Roughness:
			return FName(TEXT("G"));

		case ETextureGraphLinkedMaterialRole::Metallic:
			return FName(TEXT("B"));

		default:
			return NAME_None;
		}
	}

	bool TryGetCanonicalOutputName(UTextureGraph* TextureGraph, FName InOutputName, FName& OutCanonicalOutputName)
	{
		const UTG_Expression_Output* OutputExpression = UE::TextureGraphMaterialBridge::FindTextureGraphOutputExpression(TextureGraph, InOutputName, nullptr);
		if (!OutputExpression)
		{
			return false;
		}

		OutCanonicalOutputName = UE::TextureGraphMaterialBridge::GetTextureGraphOutputName(*OutputExpression);
		return !OutCanonicalOutputName.IsNone();
	}

	FTextureGraphMaterialPlan BuildMaterialPlan(UTextureGraph* TextureGraph)
	{
		FTextureGraphMaterialPlan Plan;
		if (!TextureGraph)
		{
			return Plan;
		}

		if (UMixSettings* Settings = TextureGraph->GetSettings())
		{
			FViewportSettings& ViewportSettings = Settings->GetViewportSettings();
			for (const FMaterialMappingInfo& MappingInfo : ViewportSettings.MaterialMappingInfos)
			{
				if (MappingInfo.Target.IsNone())
				{
					continue;
				}

				ETextureGraphLinkedMaterialRole Role = ETextureGraphLinkedMaterialRole::BaseColor;
				if (!TryGetRoleFromAlias(NormalizeAlias(MappingInfo.MaterialInput.ToString()), Role))
				{
					continue;
				}

				if (Plan.Bindings.Contains(Role))
				{
					continue;
				}

				FName CanonicalOutputName = NAME_None;
				if (!TryGetCanonicalOutputName(TextureGraph, MappingInfo.Target, CanonicalOutputName))
				{
					continue;
				}

				FTextureGraphMaterialBinding Binding;
				Binding.OutputName = CanonicalOutputName;

				const bool bIsPackedOrmOutput = IsPackedOrmAlias(NormalizeAlias(CanonicalOutputName.ToString()));
				if (bIsPackedOrmOutput && IsPackedOrmRole(Role))
				{
					Binding.OutputChannel = GetPackedOrmOutputChannelForRole(Role);
					Binding.bUsePackedOrmChannels = true;
				}
				else
				{
					Binding.OutputChannel = GetDefaultOutputChannelForRole(Role);
				}

				Plan.Bindings.Add(Role, MoveTemp(Binding));
			}
		}

		FName PackedOrmOutputName = NAME_None;
		TArray<FName> OutputNames;
		UE::TextureGraphMaterialBridge::GetTextureGraphOutputNames(TextureGraph, OutputNames);

		for (const FName OutputName : OutputNames)
		{
			const FString NormalizedOutputName = NormalizeAlias(OutputName.ToString());
			if (IsPackedOrmAlias(NormalizedOutputName))
			{
				if (PackedOrmOutputName.IsNone())
				{
					PackedOrmOutputName = OutputName;
				}

				continue;
			}

			ETextureGraphLinkedMaterialRole Role = ETextureGraphLinkedMaterialRole::BaseColor;
			if (!TryGetRoleFromAlias(NormalizedOutputName, Role))
			{
				continue;
			}

			if (Plan.Bindings.Contains(Role))
			{
				continue;
			}

			FTextureGraphMaterialBinding Binding;
			Binding.OutputName = OutputName;
			Binding.OutputChannel = GetDefaultOutputChannelForRole(Role);
			Plan.Bindings.Add(Role, MoveTemp(Binding));
		}

		if (!PackedOrmOutputName.IsNone())
		{
			for (const ETextureGraphLinkedMaterialRole Role : GetOrderedMaterialRoles())
			{
				if (!IsPackedOrmRole(Role) || Plan.Bindings.Contains(Role))
				{
					continue;
				}

				FTextureGraphMaterialBinding Binding;
				Binding.OutputName = PackedOrmOutputName;
				Binding.OutputChannel = GetPackedOrmOutputChannelForRole(Role);
				Binding.bUsePackedOrmChannels = true;
				Plan.Bindings.Add(Role, MoveTemp(Binding));
			}
		}

		return Plan;
	}

	TArray<FResolvedTextureGraphMaterialBinding> ResolveMaterialBindings(UTextureGraph* TextureGraph, const FTextureGraphMaterialPlan& Plan)
	{
		TArray<FResolvedTextureGraphMaterialBinding> ResolvedBindings;
		if (!TextureGraph)
		{
			return ResolvedBindings;
		}

		for (const ETextureGraphLinkedMaterialRole Role : GetOrderedMaterialRoles())
		{
			const FTextureGraphMaterialBinding* Binding = Plan.Bindings.Find(Role);
			if (!Binding)
			{
				continue;
			}

			if (Cast<UTexture2D>(UE::TextureGraphMaterialBridge::ResolveTextureGraphExportedTexture(TextureGraph, Binding->OutputName, nullptr)) == nullptr)
			{
				continue;
			}

			FResolvedTextureGraphMaterialBinding ResolvedBinding;
			ResolvedBinding.Role = Role;
			ResolvedBinding.Binding = *Binding;
			ResolvedBindings.Add(MoveTemp(ResolvedBinding));
		}

		return ResolvedBindings;
	}

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
		UE_LOG(LogTextureGraphMaterialBridgeMaterialCreationService, Warning, TEXT("%s"), *Message.ToString());
		ShowNotification(Message, SNotificationItem::CS_Fail);
	}

	void NotifySuccess(const FText& Message)
	{
		UE_LOG(LogTextureGraphMaterialBridgeMaterialCreationService, Display, TEXT("%s"), *Message.ToString());
		ShowNotification(Message, SNotificationItem::CS_Success);
	}

	UMaterial* CreateMaterialAsset(UTextureGraph* TextureGraph)
	{
		if (!TextureGraph)
		{
			return nullptr;
		}

		const FString TextureGraphPackageName = TextureGraph->GetOutermost()->GetName();
		const FString PackagePath = FPackageName::GetLongPackagePath(TextureGraphPackageName);
		const FString BasePackageName = FString::Printf(TEXT("%s/M_%s"), *PackagePath, *TextureGraph->GetName());

		FString UniquePackageName;
		FString UniqueAssetName;
		IAssetTools::Get().CreateUniqueAssetName(BasePackageName, TEXT(""), UniquePackageName, UniqueAssetName);

		UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
		return Cast<UMaterial>(IAssetTools::Get().CreateAsset(
			UniqueAssetName,
			FPackageName::GetLongPackagePath(UniquePackageName),
			UMaterial::StaticClass(),
			MaterialFactory,
			TEXT("TextureGraphMaterialBridge")));
	}

	UMaterialExpressionTextureGraphSample* CreateTextureGraphSampleExpression(UMaterial* Material, UTextureGraph* TextureGraph, FName OutputName, int32 NodePosX, int32 NodePosY)
	{
		UMaterialExpressionTextureGraphSample* TextureGraphSample = Cast<UMaterialExpressionTextureGraphSample>(
			UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionTextureGraphSample::StaticClass(), NodePosX, NodePosY));

		if (!TextureGraphSample)
		{
			return nullptr;
		}

		TextureGraphSample->TextureGraphAsset = TextureGraph;
		TextureGraphSample->TargetOutputName = OutputName;
		TextureGraphSample->PostEditChange();
		return TextureGraphSample;
	}

	void FinalizeMaterialAsset(UMaterial* Material)
	{
		if (!Material)
		{
			return;
		}

		UE::TextureGraphMaterialBridgeEditor::EnsureAllLiveTextureGraphTargetsInitialized();

		UMaterialEditingLibrary::LayoutMaterialExpressions(Material);
		UMaterialEditingLibrary::RecompileMaterial(Material);
		Material->PostEditChange();
		Material->MarkPackageDirty();

		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(Material->GetOutermost());
		const bool bSaved = UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);

		if (GEditor)
		{
			if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				AssetEditorSubsystem->OpenEditorForAsset(Material);
			}
		}

		const FText SuccessMessage = FText::Format(
			NSLOCTEXT("TextureGraphMaterialBridge", "LinkedMaterialCreated", "Created linked material '{0}'."),
			FText::FromString(Material->GetPathName()));
		NotifySuccess(SuccessMessage);

		if (!bSaved)
		{
			const FText SaveWarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "LinkedMaterialSaveFailed", "Created linked material '{0}', but saving the package failed."),
				FText::FromString(Material->GetName()));
			NotifyWarning(SaveWarningMessage);
		}
	}

	void CreateLinkedMaterialFromResolvedBindings(UTextureGraph* TextureGraph, const TArray<FResolvedTextureGraphMaterialBinding>& ResolvedBindings)
	{
		UMaterial* Material = CreateMaterialAsset(TextureGraph);
		if (!Material)
		{
			const FText WarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "CreateMaterialAssetFailed", "Texture Graph Material Bridge could not create a material for '{0}'."),
				FText::FromString(TextureGraph ? TextureGraph->GetName() : TEXT("None")));
			NotifyWarning(WarningMessage);
			return;
		}

		Material->Modify();

		UMaterialExpressionTextureGraphSample* PackedOrmExpression = nullptr;
		FName PackedOrmOutputName = NAME_None;
		int32 NextNodeOffsetY = 0;
		int32 SuccessfulConnections = 0;

		for (const FResolvedTextureGraphMaterialBinding& ResolvedBinding : ResolvedBindings)
		{
			UMaterialExpressionTextureGraphSample* TextureGraphSample = nullptr;

			if (ResolvedBinding.Binding.bUsePackedOrmChannels)
			{
				if (!PackedOrmExpression || PackedOrmOutputName != ResolvedBinding.Binding.OutputName)
				{
					PackedOrmExpression = CreateTextureGraphSampleExpression(Material, TextureGraph, ResolvedBinding.Binding.OutputName, -600, NextNodeOffsetY);
					PackedOrmOutputName = ResolvedBinding.Binding.OutputName;
					NextNodeOffsetY += 220;
				}

				TextureGraphSample = PackedOrmExpression;
			}
			else
			{
				TextureGraphSample = CreateTextureGraphSampleExpression(Material, TextureGraph, ResolvedBinding.Binding.OutputName, -600, NextNodeOffsetY);
				NextNodeOffsetY += 220;
			}

			if (!TextureGraphSample)
			{
				continue;
			}

			const FString OutputChannelName = ResolvedBinding.Binding.OutputChannel.IsNone() ? FString() : ResolvedBinding.Binding.OutputChannel.ToString();
			if (UMaterialEditingLibrary::ConnectMaterialProperty(TextureGraphSample, OutputChannelName, GetMaterialPropertyForRole(ResolvedBinding.Role)))
			{
				++SuccessfulConnections;
			}
		}

		if (SuccessfulConnections == 0)
		{
			const FText WarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "NoMaterialConnectionsCreated", "Texture Graph Material Bridge could not connect any material properties for '{0}'."),
				FText::FromString(TextureGraph ? TextureGraph->GetName() : TEXT("None")));
			NotifyWarning(WarningMessage);
			return;
		}

		FinalizeMaterialAsset(Material);
	}

	void HandleCreateLinkedMaterialAfterExport(UTextureGraph* TextureGraph, int32 NumExports)
	{
		if (!TextureGraph)
		{
			return;
		}

		const FTextureGraphMaterialPlan MaterialPlan = BuildMaterialPlan(TextureGraph);
		if (MaterialPlan.Bindings.IsEmpty())
		{
			const FText WarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "NoSupportedOutputsRecognized", "Texture Graph Material Bridge found no supported outputs to wire for '{0}'."),
				FText::FromString(TextureGraph->GetName()));
			NotifyWarning(WarningMessage);
			return;
		}

		TArray<FResolvedTextureGraphMaterialBinding> ResolvedBindings = ResolveMaterialBindings(TextureGraph, MaterialPlan);
		if (ResolvedBindings.IsEmpty())
		{
			const FText WarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "NoResolvedExportOutputs", "Texture Graph Material Bridge could not resolve exported 2D textures for '{0}' after export attempt ({1} exports reported)."),
				FText::FromString(TextureGraph->GetName()),
				FText::AsNumber(NumExports));
			NotifyWarning(WarningMessage);
			return;
		}

		CreateLinkedMaterialFromResolvedBindings(TextureGraph, ResolvedBindings);
	}
}

void FTextureGraphMaterialBridgeMaterialCreationService::CreateLinkedMaterials(TConstArrayView<UTextureGraph*> TextureGraphs) const
{
	for (UTextureGraph* TextureGraph : TextureGraphs)
	{
		if (!TextureGraph)
		{
			continue;
		}

		const UE::TextureGraphMaterialBridgeEditor::FResolvedTextureGraphExportSource ExportSource =
			UE::TextureGraphMaterialBridgeEditor::ResolveExportTextureGraph(TextureGraph);
		if (!ExportSource.TextureGraph)
		{
			const FText WarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "CreateLinkedMaterialNoExportSource", "Texture Graph Material Bridge could not resolve an export graph for '{0}'."),
				FText::FromString(TextureGraph->GetName()));
			UE::TextureGraphMaterialBridgeEditor::NotifyWarning(WarningMessage);
			continue;
		}

		if (ExportSource.bExportSourceDirectly)
		{
			FExportSettings ExportSettings;
			const FString SavedTextureGraphPath = TextureGraph->GetPathName();

			UE_LOG(
				LogTextureGraphMaterialBridgeMaterialCreationService,
				Log,
				TEXT("TextureGraphMaterialBridge exporting '%s' directly using %s '%s' before linked material creation."),
				*SavedTextureGraphPath,
				ExportSource.SourceDescription,
				*ExportSource.TextureGraph->GetPathName());

			FTG_HelperFunctions::ExportAsync(ExportSource.TextureGraph, TEXT(""), TEXT(""), ExportSettings, false, true, false, true)
				.then(
					[TextureGraph = TWeakObjectPtr<UTextureGraph>(TextureGraph)](int32 NumExports) mutable
			{
				AsyncTask(
					ENamedThreads::GameThread,
					[TextureGraph = MoveTemp(TextureGraph),
					 NumExports]() mutable
				{
					if (!TextureGraph.IsValid())
					{
						return;
					}

					UE::TextureGraphMaterialBridgeEditor::HandleCreateLinkedMaterialAfterExport(TextureGraph.Get(), NumExports);
				});

				return NumExports;
			});

			continue;
		}

		UTextureGraphBase* PreparedExportTextureGraph = UE::TextureGraphMaterialBridgeEditor::CreatePreparedExportTextureGraphInstance(ExportSource.TextureGraph);
		UE::TextureGraphMaterialBridgeEditor::CleanupExportTextureGraph(ExportSource.TextureGraph, ExportSource.bRequiresCleanup);
		if (!PreparedExportTextureGraph)
		{
			const FText WarningMessage = FText::Format(
				NSLOCTEXT("TextureGraphMaterialBridge", "CreateLinkedMaterialPrepareExportFailed", "Texture Graph Material Bridge could not prepare an export graph for '{0}'."),
				FText::FromString(TextureGraph->GetName()));
			UE::TextureGraphMaterialBridgeEditor::NotifyWarning(WarningMessage);
			continue;
		}

		FExportSettings ExportSettings;
		TStrongObjectPtr<UTextureGraphBase> ExportTextureGraph(PreparedExportTextureGraph);
		const FString SavedTextureGraphPath = TextureGraph->GetPathName();

		UE_LOG(
			LogTextureGraphMaterialBridgeMaterialCreationService,
			Verbose,
			TEXT("TextureGraphMaterialBridge exporting '%s' using transient instance from %s '%s' before linked material creation."),
			*SavedTextureGraphPath,
			ExportSource.SourceDescription,
			*ExportTextureGraph->GetPathName());

		UE::TextureGraphMaterialBridgeEditor::ExportPreparedTextureGraphAsync(ExportTextureGraph.Get(), ExportSettings)
			.then(
				[ExportTextureGraph = MoveTemp(ExportTextureGraph),
				 TextureGraph = TWeakObjectPtr<UTextureGraph>(TextureGraph),
				 bRequiresCleanup = true](int32 NumExports) mutable
			{
				AsyncTask(
					ENamedThreads::GameThread,
					[ExportTextureGraph = MoveTemp(ExportTextureGraph),
					 TextureGraph = MoveTemp(TextureGraph),
					 bRequiresCleanup,
					 NumExports]() mutable
				{
					UE::TextureGraphMaterialBridgeEditor::CleanupExportTextureGraph(ExportTextureGraph.Get(), bRequiresCleanup);

					if (!TextureGraph.IsValid())
					{
						return;
					}

					UE::TextureGraphMaterialBridgeEditor::HandleCreateLinkedMaterialAfterExport(TextureGraph.Get(), NumExports);
				});

				return NumExports;
			});
	}
}
