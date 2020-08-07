// Fill out your copyright notice in the Description page of Project Settings.
// Actor representing the pedestrians. Only needs a hierarchical instanced mesh component

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Pedestrian.generated.h"

/**
 * 
 */
UCLASS()
class FLOODPEDESTRIAN_API APedestrian : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	APedestrian();

	UPROPERTY(EditAnywhere, Category = "Instance")
		class UHierarchicalInstancedStaticMeshComponent* ISMComp;
};
