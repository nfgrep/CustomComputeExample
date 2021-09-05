// Author: nfgrep
// 2021

#include "CustomComputeShader.h"
#include "RenderGraph.h"
#include "RenderTargetPool.h"
#include "Engine/TextureRenderTarget2D.h"

// Associating the FCustomComputeShader class with the custom HLSL shader code
IMPLEMENT_GLOBAL_SHADER(FCustomComputeShader, "/Shaders/Private/ComputeShader.usf", "MainCompute", SF_Compute);


// This method builds and calls .Execute() on a render-graph for a single frame
void FCustomComputeShader::BuildAndExecuteGraph(FRHICommandListImmediate &RHICmdList, UTextureRenderTarget2D* RenderTarget, TArray<FVector> InVerts)
{
	// Our main point of contant with the RDG
	// We will use this to add resources (buffers) and passes to the render-graph
	FRDGBuilder GraphBuilder(RHICmdList);


	// This is a pointer to the shader-parameters we declared in the .h
	FParameters* PassParameters;
	// We ask the RDG to allocate some memory for our shader-parameters
	PassParameters = GraphBuilder.AllocParameters<FCustomComputeShader::FParameters>();


	// --- Creating an SRV filled with vertex data ---
	// 1. Create a structured buffer
	FRDGBufferRef VerticesBuffer = CreateStructuredBuffer(
		GraphBuilder,
		TEXT("Vertices_StructuredBuffer"),
		sizeof(FVector),
		InVerts.Num(),
		InVerts.GetData(),
		sizeof(FVector) * InVerts.Num()
	);
	// 2. Create an RDG-tracked SRV from our structured buffer
	FRDGBufferSRVRef VerticesSRV = GraphBuilder.CreateSRV(VerticesBuffer, PF_R32_UINT);
	// 3. Set our pass params
	PassParameters->Vertices = VerticesSRV;


	// --- Creating a texture for the compute shader to write to ---
	// 1. Make a texture description 
	FRDGTextureDesc OutTextureDesc = FRDGTextureDesc::Create2D(
		FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,
		1,
		1); 
	// 2. Allocate memory with above desc and get a ref to it
	FRDGTextureRef OutTextureRef = GraphBuilder.CreateTexture(OutTextureDesc, TEXT("Compute_Out_Texture"));
	// 3. Make a UAV description from our Texture Ref
	FRDGTextureUAVDesc OutTextureUAVDesc(OutTextureRef);
	// 4. Initialize it as our OutputTexture in our pass params
	PassParameters->OutputTexture = GraphBuilder.CreateUAV(OutTextureUAVDesc);


	// ------ Add the compute pass ------
	// Get a reference to our global shader class
	TShaderMapRef<FCustomComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	// Add the compute shader pass to the render graph
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Compute Pass"), ComputeShader, PassParameters, FIntVector(32, 32, 1));


	// ------ Extracting to pooled render target ------
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget;
	// Copy the result of compute shader from UAV to pooled renderT-target
	GraphBuilder.QueueTextureExtraction(OutTextureRef, &PooledComputeTarget);

	// Execute the graph
	GraphBuilder.Execute();

	// Queue the UAV we wrote to for extraction 
	RHICmdList.CopyToResolveTarget(PooledComputeTarget.GetReference()->GetRenderTargetItem().TargetableTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());
}

