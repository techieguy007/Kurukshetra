using UnrealBuildTool;

public class Kurukshetra : ModuleRules
{
    public Kurukshetra(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] {
            "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore"
        });
    }
}
