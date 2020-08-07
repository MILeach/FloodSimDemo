// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"

// Sets default values
ACamera::ACamera()
{
	cameraSpeed = 1.0f;
	cameraFOV = 90.0f;
	minimumFOV = 60.0f;
	maximumFOV = 120.0f;

	bRightClick = false;

	//add camera components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	cameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	cameraSpringArm->SetupAttachment(RootComponent);
	cameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(0.0f, 0.0f, 0.0f));
	cameraSpringArm->TargetArmLength = 0.0f;
	cameraSpringArm->bEnableCameraLag = true;
	cameraSpringArm->CameraLagSpeed = 0.0f;
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	camera->SetupAttachment(cameraSpringArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();
	
	camera->FieldOfView = cameraFOV;
	//get controoler
	controller = GetWorld()->GetFirstPlayerController();
	//show cursor
	ShowCursor();
}

// Called to bind functionality to input
void ACamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Camera
	PlayerInputComponent->BindAxis("MoveForward", this, &ACamera::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACamera::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ACamera::MoveUp);
	PlayerInputComponent->BindAxis("CameraPitch", this, &ACamera::CameraPitch);
	PlayerInputComponent->BindAxis("CameraYaw", this, &ACamera::CameraYaw);
	PlayerInputComponent->BindAxis("Zoom", this, &ACamera::Zoom);

	//Right Click
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &ACamera::RightClickPressed);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &ACamera::RightClickReleased);

}

void ACamera::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		FVector newLocation = GetActorLocation();
		newLocation += cameraSpringArm->GetForwardVector() * Value * cameraSpeed * 10.0f;
		SetActorLocation(newLocation);
	}
}

void ACamera::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		FVector newLocation = GetActorLocation();
		newLocation += GetActorRightVector() * Value * cameraSpeed * 10.0f;
		SetActorLocation(newLocation);
	}
}

void ACamera::MoveUp(float Value)
{
	if (Value != 0.0f)
	{
		FVector newLocation = GetActorLocation();
		FVector up = { 0.0, 0.0, 1.0 };
		newLocation += up * Value * cameraSpeed * 10.0f;
		SetActorLocation(newLocation);
	}
}

void ACamera::CameraPitch(float Value)
{
	if (Value != 0.0f && bRightClick)
	{
		FRotator newRotation = cameraSpringArm->GetComponentRotation();
		newRotation.Pitch = FMath::Clamp(newRotation.Pitch + Value, -89.0f, 89.0f);
		cameraSpringArm->SetWorldRotation(newRotation);
	}
}

void ACamera::CameraYaw(float Value)
{
	if (Value != 0.0f && bRightClick)
	{
		FRotator newRotation = GetActorRotation();
		newRotation.Yaw += Value * 1.5f;
		SetActorRotation(newRotation);
	}
}

void ACamera::Zoom(float Value)
{
	if (Value != 0.0f)
	{
		cameraFOV += Value * 1.0f;
		cameraFOV = FMath::Clamp(cameraFOV, minimumFOV, maximumFOV);
		camera->FieldOfView = cameraFOV;
	}
}

void ACamera::RightClickPressed()
{
	bRightClick = true;
	HideCursor();
}

void ACamera::RightClickReleased()
{
	bRightClick = false;
	ShowCursor();
}

void ACamera::ShowCursor()
{
	FInputModeGameAndUI UIMode;
	UIMode.SetHideCursorDuringCapture(false);
	controller->SetInputMode(UIMode);
	controller->bShowMouseCursor = true;
	controller->bEnableClickEvents = true;
	controller->bEnableMouseOverEvents = true;
}

void ACamera::HideCursor()
{
	FInputModeGameOnly gameMode;
	controller->SetInputMode(gameMode);
	controller->bShowMouseCursor = false;
	controller->bEnableClickEvents = false;
	controller->bEnableMouseOverEvents = false;
}