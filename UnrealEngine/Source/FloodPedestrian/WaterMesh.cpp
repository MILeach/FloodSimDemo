// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterMesh.h"

// Sets default values
AWaterMesh::AWaterMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	waterMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("waterMeshComponent");
	SetRootComponent(waterMeshComponent);
	waterMeshComponent->bUseAsyncCooking = true;

}

// Called when the game starts or when spawned
void AWaterMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaterMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

