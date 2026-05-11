using UnrealBuildTool;

public class TextureGraphMaterialBridge : ModuleRules
{
    public TextureGraphMaterialBridge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bool bWithDesignerNodes = Target.bBuildEditor;

        PrivateDefinitions.Add(bWithDesignerNodes ? "TGMB_WITH_DESIGNER_NODES=1" : "TGMB_WITH_DESIGNER_NODES=0");

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "TextureGraph"
            });

        if (bWithDesignerNodes)
        {
            PrivateDependencyModuleNames.AddRange(
                new[]
                {
                    "Projects",
                    "RenderCore",
                    "RHI",
                    "TextureGraphEngine"
                });
        }
    }
}
