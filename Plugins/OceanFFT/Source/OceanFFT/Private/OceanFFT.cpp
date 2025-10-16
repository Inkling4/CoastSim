// OceanFFT.cpp

#include "OceanFFT.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FOceanFFTModule"

void FOceanFFTModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
    
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("OceanFFT"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/OceanFFT"), PluginShaderDir);
}

void FOceanFFTModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FOceanFFTModule, OceanFFT)