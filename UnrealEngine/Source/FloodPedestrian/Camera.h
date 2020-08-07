// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Camera.generated.h"

UCLASS()
class FLOODPEDESTRIAN_API ACamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	APlayerController* controller;

	//camera
	UPROPERTY(EditAnywhere)
		USpringArmComponent* cameraSpringArm;
	UPROPERTY(EditAnywhere)
		UCameraComponent* camera;

	float cameraFOV;
	float minimumFOV, maximumFOV;
	float cameraSpeed;

	bool bRightClick;

	//camera control
	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);
	void CameraPitch(float Value);
	void CameraYaw(float Value);
	void Zoom(float Value);

	//right click
	void RightClickPressed();
	void RightClickReleased();

	//cursor
	void ShowCursor();
	void HideCursor();

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
