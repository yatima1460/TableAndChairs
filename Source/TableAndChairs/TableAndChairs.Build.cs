using UnrealBuildTool;

public class TableAndChairs : ModuleRules
{
	public TableAndChairs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //Always create a precompiled header
        MinFilesUsingPrecompiledHeaderOverride = 1;

        //This will turn off the combining of source files
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "ProceduralMeshComponent"
        });

	}
}
