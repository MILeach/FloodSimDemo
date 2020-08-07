// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProceduralMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterMesh.generated.h"


UCLASS()
class FLOODPEDESTRIAN_API AWaterMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	UProceduralMeshComponent* waterMeshComponent;
	virtual void Tick(float DeltaTime) override;

};
