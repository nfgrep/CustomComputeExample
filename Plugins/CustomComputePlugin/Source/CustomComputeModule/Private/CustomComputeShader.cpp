

#include "CustomComputeShader.h"
#include "RenderGraph.h"
#include "RenderTargetPool.h"
#include "Engine/TextureRenderTarget2D.h"

IMPLEMENT_GLOBAL_SHADER(FCustomComputeShader, "/Shaders/Private/ComputeShader.usf", "MainCompute", SF_Compute);

void FCustomComputeShader::BuildAndExecuteGraph(FRHICommandListImmediate &RHICmdList, UTextureRenderTarget2D* RenderTarget, TArray<FVector> InVerts)
{
	FRDGBuilder GraphBuilder(RHICmdList);

	FParameters* PassParameters;
	PassParameters = GraphBuilder.AllocParameters<FCustomComputeShader::FParameters>();


	// --- Creating a UAV to pass in our detector pixel positions ---
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
	//FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(DrawParameters.GetRenderTargetSize().X, THREADS_PER_GROUP_DIMEN), FMath::DivideAndRoundUp(DrawParameters.GetRenderTargetSize().Y, THREADS_PER_GROUP_DIMEN), 1);
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

