// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomComputePlugin.h"
#include "CustomComputeShader.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h" 

#define LOCTEXT_NAMESPACE "FCustomComputePluginModule"

void FCustomComputePluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CustomComputePlugin"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Shaders"), PluginShaderDir);
}

void FCustomComputePluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FCustomComputePluginModule::EnqueueRenderCommand(UTextureRenderTarget2D* RenderTarget)
{
	TShaderMapRef<FCustomComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	ENQUEUE_RENDER_COMMAND(ComputeShader)(
		[
			ComputeShader,
			RenderTarget
		](FRHICommandListImmediate& RHICmdList)
		{
			ComputeShader->BuildAndExecuteGraph(
				RHICmdList,
				RenderTarget);
		});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomComputePluginModule, CustomComputePlugin)