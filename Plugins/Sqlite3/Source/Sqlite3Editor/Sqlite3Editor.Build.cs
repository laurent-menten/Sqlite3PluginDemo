// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Sqlite3Editor : ModuleRules
{
	public Sqlite3Editor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange( new string[]
		{
		} );
				
		PrivateIncludePaths.AddRange( new string[]
		{
		} );
			
		PublicDependencyModuleNames.AddRange( new string[]
		{
			"Core", "DataValidation",
        });
			
		PrivateDependencyModuleNames.AddRange( new string[]
		{
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",

			"UnrealEd",
			"AssetTools",

            "Sqlite3",
        });
		
		DynamicallyLoadedModuleNames.AddRange( new string[]
		{
		} );
    }
}
