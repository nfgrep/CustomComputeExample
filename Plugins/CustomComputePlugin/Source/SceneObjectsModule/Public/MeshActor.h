// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomComputeModule.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshActor.generated.h"


UCLASS()
class SCENEOBJECTSMODULE_API AMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeshActor();

	// The mesh we'll get the data from
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ExampleMesh;

	// The render-target we'll pass onto the GPU to be written to
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	TArray<FVector> GetVerts(AActor* Actor, int32 ActorLOD);

	FCustomComputeModule ComputeModule;

};
