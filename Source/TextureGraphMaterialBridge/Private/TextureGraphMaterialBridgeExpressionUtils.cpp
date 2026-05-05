#include "TextureGraphMaterialBridgeExpressionUtils.h"

#include "Engine/Texture.h"
#include "Expressions/Output/TG_Expression_Output.h"
#include "TG_Graph.h"
#include "TG_Node.h"
#include "TG_OutputSettings.h"
#include "TextureGraph.h"

namespace UE::TextureGraphMaterialBridge
{
	namespace
	{
		const UTG_Graph* GetInitializedTextureGraph(const UTextureGraphBase* TextureGraph)
		{
			if (!TextureGraph)
			{
				return nullptr;
			}

			if (const UTextureGraphInstance* TextureGraphInstance = Cast<UTextureGraphInstance>(TextureGraph))
			{
				if (!TextureGraphInstance->Graph())
				{
					UTextureGraphInstance* MutableTextureGraphInstance = const_cast<UTextureGraphInstance*>(TextureGraphInstance);
					MutableTextureGraphInstance->Initialize();
				}
			}

			return TextureGraph->Graph();
		}
	}

	FName GetTextureGraphOutputName(const UTG_Expression_Output& OutputExpression)
	{
		return OutputExpression.GetTitleName();
	}

	void GetTextureGraphOutputNames(const UTextureGraphBase* TextureGraph, TArray<FName>& OutOutputNames)
	{
		OutOutputNames.Reset();

		if (!TextureGraph)
		{
			return;
		}

		const UTG_Graph* Graph = GetInitializedTextureGraph(TextureGraph);
		if (!Graph)
		{
			return;
		}

		Graph->ForEachNodes([&OutOutputNames](const UTG_Node* Node, uint32)
		{
			if (!Node)
			{
				return;
			}

			if (const UTG_Expression_Output* OutputExpression = Cast<UTG_Expression_Output>(Node->GetExpression()))
			{
				OutOutputNames.AddUnique(GetTextureGraphOutputName(*OutputExpression));
			}
		});
	}

	const UTG_Expression_Output* FindTextureGraphOutputExpression(const UTextureGraphBase* TextureGraph, FName InOutputName, FString* OutError)
	{
		if (!TextureGraph)
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(TEXT("Select a Texture Graph source asset on the node."), nullptr, InOutputName);
			}
			return nullptr;
		}

		if (InOutputName.IsNone())
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(TEXT("Select an output name on the node."), TextureGraph, InOutputName);
			}
			return nullptr;
		}

		const UTG_Graph* Graph = GetInitializedTextureGraph(TextureGraph);
		if (!Graph)
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(TEXT("The Texture Graph source asset has no loaded graph data."), TextureGraph, InOutputName);
			}
			return nullptr;
		}

		const UTG_Expression_Output* FoundOutput = nullptr;
		Graph->ForEachNodes([InOutputName, &FoundOutput](const UTG_Node* Node, uint32)
		{
			if (FoundOutput || !Node)
			{
				return;
			}

			if (const UTG_Expression_Output* OutputExpression = Cast<UTG_Expression_Output>(Node->GetExpression()))
			{
				const FName CanonicalOutputName = GetTextureGraphOutputName(*OutputExpression);
				if (CanonicalOutputName == InOutputName || OutputExpression->GetTitleName() == InOutputName || OutputExpression->OutputSettings.OutputName == InOutputName)
				{
					FoundOutput = OutputExpression;
				}
			}
		});

		if (!FoundOutput && OutError)
		{
			*OutError = BuildTextureGraphMaterialUsageError(
				FString::Printf(TEXT("No output named '%s' exists on the Texture Graph."), *InOutputName.ToString()),
				TextureGraph,
				InOutputName);
		}

		return FoundOutput;
	}

	UTexture* ResolveTextureGraphExportedTexture(const UTextureGraphBase* TextureGraph, FName InOutputName, FString* OutError)
	{
		const UTG_Expression_Output* OutputExpression = FindTextureGraphOutputExpression(TextureGraph, InOutputName, OutError);
		if (!OutputExpression)
		{
			return nullptr;
		}

		const FName CanonicalOutputName = GetTextureGraphOutputName(*OutputExpression);
		const FTG_OutputSettings& OutputSettings = OutputExpression->OutputSettings;
		if (!OutputSettings.bShouldExport)
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(
					FString::Printf(TEXT("Enable export for output '%s' in the Texture Graph before using it in materials."), *CanonicalOutputName.ToString()),
					TextureGraph,
					CanonicalOutputName);
			}
			return nullptr;
		}

		if (OutputSettings.FolderPath.IsNone())
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(
					FString::Printf(TEXT("Set a valid export folder for output '%s' in the Texture Graph."), *CanonicalOutputName.ToString()),
					TextureGraph,
					CanonicalOutputName);
			}
			return nullptr;
		}

		if (OutputSettings.BaseName.IsNone())
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(
					FString::Printf(TEXT("Set a valid export file name for output '%s' in the Texture Graph."), *CanonicalOutputName.ToString()),
					TextureGraph,
					CanonicalOutputName);
			}
			return nullptr;
		}

		FString PathErrors;
		if (!OutputSettings.Validate(PathErrors))
		{
			if (OutError)
			{
				*OutError = BuildTextureGraphMaterialUsageError(
					FString::Printf(TEXT("The configured export path for output '%s' is invalid (%s)."), *CanonicalOutputName.ToString(), *PathErrors),
					TextureGraph,
					CanonicalOutputName);
			}
			return nullptr;
		}

		const FString TextureObjectPath = BuildTextureGraphExportedTextureObjectPath(OutputSettings);
		UTexture* Texture = LoadObject<UTexture>(nullptr, *TextureObjectPath);
		if (!Texture && OutError)
		{
			*OutError = BuildTextureGraphMaterialUsageError(
				FString::Printf(TEXT("No exported texture exists at '%s'. Save the Texture Graph to regenerate that exported asset."), *TextureObjectPath),
				TextureGraph,
				CanonicalOutputName);
		}

		return Texture;
	}

	FString BuildTextureGraphExportedTextureObjectPath(const FTG_OutputSettings& OutputSettings)
	{
		FString FolderPath = OutputSettings.FolderPath.ToString();
		FolderPath.ReplaceInline(TEXT("\\"), TEXT("/"));

		while (FolderPath.EndsWith(TEXT("/")))
		{
			FolderPath.LeftChopInline(1, EAllowShrinking::No);
		}

		const FString AssetName = OutputSettings.BaseName.ToString();
		const FString PackagePath = FString::Printf(TEXT("%s/%s"), *FolderPath, *AssetName);
		return FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);
	}

	FString BuildTextureGraphMaterialUsageError(const FString& DetailMessage, const UTextureGraphBase* InTextureGraph, FName InOutputName)
	{
		const FString SourceName = InTextureGraph ? InTextureGraph->GetName() : TEXT("None");
		const FString OutputLabel = InOutputName.IsNone() ? TEXT("None") : InOutputName.ToString();
		return FString::Printf(
			TEXT("Texture Graph output is not ready for material use. %s Source='%s', Output='%s'."),
			*DetailMessage,
			*SourceName,
			*OutputLabel);
	}
}
