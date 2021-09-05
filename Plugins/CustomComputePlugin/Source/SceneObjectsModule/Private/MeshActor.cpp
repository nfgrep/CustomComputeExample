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

	ComputeModule.EnqueueRenderCommand(RenderTarget, GetVerts(this));
}

// Returns the vertices for the given actor in world-space
TArray<FVector> AMeshActor::GetVerts(AActor* Actor)
{
	// The array we'll return
	TArray<FVector> MeshVerts;

	// Get a static mesh from the first component
	TArray<UStaticMeshComponent*> StaticMeshComponents = TArray<UStaticMeshComponent*>();
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
	UStaticMesh* StaticMesh = StaticMeshComponents[0]->GetStaticMesh();

	// Check if the static mesh is null
	if (!StaticMesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("StaticMesh[0] was null for actor: ")).Append(Actor->GetName()));
		return MeshVerts;
	}

	// Check if this static mesh has a LOD 
	if (!(StaticMesh->GetRenderData()->LODResources.Num() > 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Each mesh must have the supplied LOD To Use"));
		return MeshVerts;
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
		MeshVerts.Add(VertexWS);	// NOTE: .Add can be pretty slow!
	}

	return MeshVerts;
}


