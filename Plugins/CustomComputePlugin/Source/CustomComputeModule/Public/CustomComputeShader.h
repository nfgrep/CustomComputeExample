// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

// This is the class that represents our shader within the engine
// This class is associated with shader-code in the corresponding .cpp
class FCustomComputeShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCustomComputeShader)
	SHADER_USE_PARAMETER_STRUCT(FCustomComputeShader, FGlobalShader)
		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, ) 

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FVector>, Vertices)

		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<FVector4>, OutputTexture)

	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	void BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, UTextureRenderTarget2D* RenderTarget, TArray<FVector> InVerts);
};