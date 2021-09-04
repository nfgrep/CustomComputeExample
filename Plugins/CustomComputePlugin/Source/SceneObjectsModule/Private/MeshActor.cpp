// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshActor.h"


// Sets default values
AMeshActor::AMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup a static mesh component for this actor
	ExampleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Example Mesh"));
	// Set the static mesh to the cube included with the engine
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	ExampleMesh->SetStaticMesh(CubeMesh.Object);
	// Make the static mesh component the root of this actor
	RootComponent = ExampleMesh;

	// Get a reference to the CustomeComputeModule
	ComputeModule = FModuleManager::LoadModuleChecked<FCustomComputeModule>("CustomComputeModule");
}

void AMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ComputeModule.EnqueueRenderCommand(RenderTarget, GetVerts(this, 0));

}

// Returns the vertices for the given actor in world-space
TArray<FVector> AMeshActor::GetVerts(AActor* Actor, int32 ActorLOD)
{
	TArray<FVector> SMVertices;

	// Get the static mesh from the first static mesh component
	TArray<UStaticMeshComponent*> StaticMeshComponents = TArray<UStaticMeshComponent*>();
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	UStaticMesh* StaticMesh = StaticMeshComponents[0]->GetStaticMesh();

	if (!StaticMesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("StaticMesh[0] was null for actor: ")).Append(Actor->GetName()));
		return SMVertices;
	}

	// Check if this static mesh has the required LOD
	if (!(StaticMesh->GetRenderData()->LODResources.Num() > ActorLOD))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Each mesh must have the supplied LOD To Use"));
		return SMVertices;
	}

	// Get the vertices
	FPositionVertexBuffer* VertexBuffer = &StaticMesh->GetRenderData()->LODResources[ActorLOD].VertexBuffers.PositionVertexBuffer;
	for (uint32 VertIdx = 0; VertIdx < VertexBuffer->GetNumVertices(); VertIdx++)
	{
		// Get this vertex
		FVector VertexLS = VertexBuffer->VertexPosition(VertIdx);
		
		// Transform from local to world space
		FVector VertexWS = Actor->GetTransform().TransformPosition(VertexLS);

		// Add it to the array we'll return
		SMVertices.Add(VertexWS);	// NOTE: .Add can be pretty slow!
	}

	return SMVertices;
}


