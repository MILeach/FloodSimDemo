// Fill out your copyright notice in the Description page of Project Settings.


#include "Pedestrian.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

APedestrian::APedestrian()
{
	ISMComp = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	ISMComp->SetupAttachment(RootComponent);

	ISMComp->bOwnerNoSee = false;
	ISMComp->bCastDynamicShadow = false;
	ISMComp->CastShadow = false;
	ISMComp->SetHiddenInGame(false);
	ISMComp->CanCharacterStepUpOn = ECB_No;
	ISMComp->SetMobility(EComponentMobility::Movable);
}
