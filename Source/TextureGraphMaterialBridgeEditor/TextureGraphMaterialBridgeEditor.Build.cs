using UnrealBuildTool;

public class TextureGraphMaterialBridgeEditor : ModuleRules
{
    public TextureGraphMaterialBridgeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "AssetRegistry",
                "AssetTools",
                "ContentBrowser",
                "Core",
                "CoreUObject",
                "Engine",
                "GraphEditor",
                "InputCore",
                "MaterialEditor",
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "TextureGraph",
                "TextureGraphEditor",
                "TextureGraphEngine",
                "TextureGraphMaterialBridge",
                "UnrealEd"
            });
    }
}
