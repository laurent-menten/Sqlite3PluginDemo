using UnrealBuildTool;

public class Sqlite3Tools : ModuleRules
{
    public Sqlite3Tools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",

                "UnrealEd",
                "GraphEditor",
                "BlueprintGraph",
                "KismetCompiler",

                "Sqlite3",
            }
        );
    }
}