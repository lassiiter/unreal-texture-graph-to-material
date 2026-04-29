using UnrealBuildTool;

public class TextureGraphMaterialBridge : ModuleRules
{
    public TextureGraphMaterialBridge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "TextureGraph"
            });
    }
}
