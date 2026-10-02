using UnrealBuildTool;
using System.Collections.Generic;

public class KurukshetraTarget : TargetRules
{
    public KurukshetraTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Kurukshetra");
    }
}
