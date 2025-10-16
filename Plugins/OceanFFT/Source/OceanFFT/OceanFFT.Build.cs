// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class OceanFFT : ModuleRules
{
    public OceanFFT(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RenderCore",  // Add this for shader support
                "RHI",         // Add this for shader support
                "Projects",    // Add this for shader directory mapping
                // ... add other public dependencies that you statically link with here ...
            }
        );
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Niagara",     // Add this since you're using Niagara
                "NiagaraCore", // Add this since you're using Niagara
                // ... add private dependencies that you statically link with here ...
            }
        );
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
        );
        
        // ADD THIS SECTION - This registers your shader directory
        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "UnrealEd",
                }
            );
        }
        
        // Register custom shader directory - CORRECTED VERSION
        string PluginShaderDir = Path.Combine(ModuleDirectory, "..", "Shaders");
        PublicIncludePaths.Add(Path.GetFullPath(PluginShaderDir));
    }
}