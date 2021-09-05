// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomComputeModule.h"
#include "CustomComputeShader.h"
#include "Interfaces/IPluginManager.h"
#include "GlobalShader.h"
#include "ShaderCore.h" 

#define LOCTEXT_NAMESPACE "FCustomComputeModule"

void FCustomComputeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CustomComputePlugin"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Shaders"), PluginShaderDir);
}

void FCustomComputeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


void FCustomComputeModule::EnqueueRenderCommand(UTextureRenderTarget2D* RenderTarget, TArray<FVector> Vertices)
{
	TShaderMapRef<FCustomComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	UTextureRenderTarget2D* RenderTargetParam = RenderTarget;
	TArray<FVector> VerticesParam = Vertices;

	ENQUEUE_RENDER_COMMAND(ComputeShader)(
		[
			ComputeShader,
			RenderTargetParam,
			VerticesParam
		](FRHICommandListImmediate& RHICmdList)
		{
			ComputeShader->BuildAndExecuteGraph(
				RHICmdList,
				RenderTargetParam,
				VerticesParam);
		});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomComputeModule, CustomComputeModule)