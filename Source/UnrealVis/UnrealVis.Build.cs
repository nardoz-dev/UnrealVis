// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class UnrealVis : ModuleRules
{
	public UnrealVis(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore" , 
            "UMG", 
            "Slate", 
            "SlateCore",
            "RenderCore",
            "RHI",
            "Json",
            "JsonUtilities",
            "HTTP"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Dice al linker di usare la libreria di sistema per le performance
            PublicSystemLibraries.Add("pdh.lib");
        }

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
