// Copyright Epic Games, Inc. All Rights Reserved.


#include "FloodPedestrianGameModeBase.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "glm.hpp"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include <winbase.h>
#include "Blueprint/UserWidget.h"
#include <fstream>

#undef UpdateResource

AFloodPedestrianGameModeBase::AFloodPedestrianGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	simulationStarted = false;
	firstFrame = true;
}

void AFloodPedestrianGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//display hud
	if (menuWidgetClass != nullptr)
	{
		menuWidget = CreateWidget<UUserWidget>(GetWorld(), menuWidgetClass);
		if (menuWidget != nullptr)
		{
			menuWidget->AddToViewport();
			menuWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}

	//alloc memory for communication structs
	pedestrians = (pedestrian_data*)malloc(sizeof(pedestrian_data));
	floodCells = (flood_data*)malloc(sizeof(flood_data));
	simulationStateData = (simulation_state_data*)malloc(sizeof(simulation_state_data));
	options = (options_data*)malloc(sizeof(options_data));

	//fill materials array LOD 0
	materials.Add(pedestrianMaterial00);
	materials.Add(pedestrianMaterial10);

	//fill materials LOD 1 array
	materialsLOD1.Add(pedestrianMaterialLOD100);
	materialsLOD1.Add(pedestrianMaterialLOD110);

	//fill materials LOD 2 array
	materialsLOD2.Add(pedestrianMaterialLOD200);
	materialsLOD2.Add(pedestrianMaterialLOD210);

	//fill wigs array LOD0
	wigs.Add(wigMaterial10);

	//fill wigs array LOD1
	wigsLOD1.Add(wigMaterialLOD110);

	//fill wigs array LOD2
	wigsLOD2.Add(wigMaterialLOD210);

	// spawn pedestrian actor for instances
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	FRotator spawnRotation = { 0.0f, 0.0f, 0.0f };
	FVector spawnLocation = { 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < NUM_MODELS; i++)
	{
		APedestrian* pedestrian;
		pedestrian = GetWorld()->SpawnActor<APedestrian>(APedestrian::StaticClass(), spawnLocation, spawnRotation, spawnParams);
		pedestrian->ISMComp[0].SetMobility(EComponentMobility::Movable);

		//models
		if (i == 0)
		{
			pedestrian->ISMComp->SetStaticMesh(SMPedestrian0);
			pedestrian->ISMComp->SetMaterial(0, materials[i]);
			pedestrian->ISMComp->SetMaterial(1, materialsLOD1[i]);
			pedestrian->ISMComp->SetMaterial(2, materialsLOD2[i]);
		}
		else
		{
			pedestrian->ISMComp->SetStaticMesh(SMPedestrian1);
			pedestrian->ISMComp->SetMaterial(0, materials[i]);
			pedestrian->ISMComp->SetMaterial(1, wigs[0]);
			pedestrian->ISMComp->SetMaterial(2, materialsLOD1[i]);
			pedestrian->ISMComp->SetMaterial(3, wigsLOD1[0]);
			pedestrian->ISMComp->SetMaterial(4, materialsLOD2[i]);
			pedestrian->ISMComp->SetMaterial(5, wigsLOD2[0]);
		}
		agents.Add(pedestrian);
	}



	
	
}

void AFloodPedestrianGameModeBase::SpawnTopographyAndWater() {
	// Select correct water map
	FString waterMapPath = "/Game/SimulationMaps/" + SimulationFolder + "/water_map";
	const TCHAR* wmp = *waterMapPath;
	SMMap = LoadObject<UStaticMesh>(nullptr, wmp);

	// Set correct topology
	FString topographyPath = "/Game/SimulationMaps/" + SimulationFolder + "/map";
	const TCHAR* tp = *topographyPath;
	TopographyMesh = LoadObject<UStaticMesh>(nullptr, tp);

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	FRotator spawnRotation = { 0.0f, 0.0f, 0.0f };
	FVector spawnLocation = { 0.0f, 0.0f, 0.0f };

	// Spawn water plane
	spawnLocation.X = 250.0f;
	spawnLocation.Y = 250.0f; //change this to 250.0f to be on top of the map
	spawnLocation.Z = -0.1f;
	FTransform tempTransform;
	tempTransform.SetLocation(spawnLocation);
	tempTransform.SetScale3D({ 250.0f, -250.0f, zScale });
	FRotator rotation = FRotator(0, 90.0, 0);
	tempTransform.SetRotation(FQuat(rotation));
	//map = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), tempTransform, spawnParams);
	waterMesh = GetWorld()->SpawnActor<AWaterMesh>(AWaterMesh::StaticClass(), tempTransform, spawnParams);
	waterMesh->waterMeshComponent->SetMaterial(0, waterMaterial); //use this for material
	loadWaterMesh();
	previousDisplacement.Init(0.0f, xmachine_memory_agent_MAX);

	// Spawn topography
	spawnLocation.Z = 0.0f;
	tempTransform.SetLocation(spawnLocation);
	tempTransform.SetScale3D({ 1.0f, 1.0f, zScale/250.0f });
	topography = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), tempTransform, spawnParams);
	topography->SetMobility(EComponentMobility::Movable);
	topography->GetStaticMeshComponent()->SetStaticMesh(TopographyMesh);
}

void AFloodPedestrianGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (simulationStarted)
	{
		//communication with FLAME
		Communication();
	}
}

void AFloodPedestrianGameModeBase::UpdateFolderPath(FString newPath) {
	SimulationFolder = newPath;
}

void AFloodPedestrianGameModeBase::setZScale(float newZScale) {
	zScale = newZScale;
}

void AFloodPedestrianGameModeBase::StartSimulation()
{
	SpawnTopographyAndWater();

	//get simulation path
	FString directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	// Release_Visualisation\PedestrianNavigation.exe "..\..\examples\FloodPedestrian_SW_stadium\iterations\map.xml"
	FString name = "PedestrianNavigation.bat";

	// Editor uses one less directory level than build
#if WITH_EDITOR
	FString path = FPaths::Combine(directory, TEXT("../FLAMEGPU-development/bin/x64/"));
#else
	FString path = FPaths::Combine(directory, TEXT("../../FLAMEGPU-development/bin/x64/"));
#endif

	const TCHAR* simFolder = *SimulationFolder;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, path);

	//set simulation started
	simulationStarted = true;

	// initialise memory segments
	TCHAR pedestrianName[] = TEXT("GraphSizeFileMapping");
	TCHAR floodName[] = TEXT("PositionFileMapping");
	TCHAR simulationStateName[] = TEXT("SimulationStateFileMapping");
	TCHAR optionsName[] = TEXT("OptionsFileMapping");

	// create handles
	hPedestrianMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(pedestrian_data), pedestrianName);
	hFloodMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(flood_data), floodName);
	hSimulationStateMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(simulation_state_data), simulationStateName);
	hOptionsMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(options_data), optionsName);

	if (hPedestrianMapFile == NULL || hFloodMapFile == NULL || hSimulationStateMapFile == NULL || hOptionsMapFile == NULL)
	{
		exit(0);
	}

	// map buffers
	pedestrianBuf = (LPTSTR)MapViewOfFile(hPedestrianMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(pedestrian_data));
	floodBuf = (LPTSTR)MapViewOfFile(hFloodMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(flood_data));
	simulationStateBuf = (LPTSTR)MapViewOfFile(hSimulationStateMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(simulation_state_data));
	optionsBuf = (LPTSTR)MapViewOfFile(hOptionsMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(options_data));

	if (pedestrianBuf == NULL)
	{
		CloseHandle(hPedestrianMapFile);
		exit(0);
	}
	if (floodBuf == NULL)
	{
		CloseHandle(hFloodMapFile);
		exit(0);
	}
	if (simulationStateBuf == NULL)
	{
		CloseHandle(hSimulationStateMapFile);
		exit(0);
	}
	if (optionsBuf == NULL)
	{
		CloseHandle(hOptionsMapFile);
		exit(0);
	}

	//start process
	handle = FPlatformProcess::CreateProc(*(path + name), simFolder, false, true, true, NULL, 0, *path, NULL, NULL);
}

void AFloodPedestrianGameModeBase::StopSimulation()
{
	//send 'quit' to FLAME to quit the simulation
	options->quit = 1;
	CopyMemory((PVOID)optionsBuf, options, sizeof(options_data));

	//reset data
	if (topography)
		topography->Destroy();
	if (waterMesh)
		waterMesh->Destroy();
	waterVerts.Empty();
	waterTris.Empty();
	initialVertexPositions.Empty();
	simulationStarted = false;
	options->quit = 0;
	memset((PVOID)pedestrianBuf, 0, sizeof(pedestrian_data));
	memset((PVOID)floodBuf, 0, sizeof(flood_data));
	memset((PVOID)simulationStateBuf, 0, sizeof(simulation_state_data));
	previousDisplacement.Empty();
	previousDisplacement.Init(0.0f, xmachine_memory_agent_MAX);
	ClearPedestrians();
	FPlatformProcess::CloseProc(handle);

}

void AFloodPedestrianGameModeBase::Quit()
{
	options->quit = 1;
	CopyMemory((PVOID)optionsBuf, options, sizeof(options_data));

	//exit Unreal application
	FGenericPlatformMisc::RequestExit(false);
}

void AFloodPedestrianGameModeBase::Communication()
{
	//get flame data
	CopyMemory(options, optionsBuf, sizeof(int));

	if (options->lock == 1)
	{
		CopyMemory(pedestrians, pedestrianBuf, sizeof(pedestrian_data));
		CopyMemory(floodCells, floodBuf, sizeof(flood_data));
		CopyMemory(simulationStateData, simulationStateBuf, sizeof(simulation_state_data));
		options->lock = 0;
		CopyMemory((PVOID)optionsBuf, options, sizeof(int));
		ClearPedestrians();
		SpawnPedestrians();
		UpdateWaterLevel();
	}
}

void AFloodPedestrianGameModeBase::SpawnPedestrians()
{
	//each frame pedestrians are deleted and created again with the new data
	

	for (int i = 0; i < pedestrians->total; i++)
	{
		//ignore corrupt data
		if (isnan(pedestrians->x[i]) || isnan(pedestrians->y[i]) || isnan(pedestrians->z[i]))
			continue;
		
		//position
		glm::vec3 loc = glm::vec3(pedestrians->x[i], pedestrians->y[i], pedestrians->z[i]);
		// Convert location to index
		uint32 iy = static_cast<uint32>(((1.0f + loc.y) / 2.0f) * 128.0f);
		uint32 ix = static_cast<uint32>(((1.0f + loc.x) / 2.0f) * 128.0f);
		std::cout << iy << " " << ix << " " << iy * 128 + ix;
		loc = (loc + glm::vec3(1.0f)) * SCALE;
			
		FVector spawnLocation = { loc.y, loc.x, 0.0f};
		spawnLocation.Z = initialVertexPositions[ix * 128 + iy].Z *zScale;

		/*FHitResult OutHit;
		FVector rayStart = { loc.x, loc.y, 1000.0f };
		FVector rayDirection = { 0.0f, 0.0f, -1.0f };
		FVector rayEnd = rayStart + rayDirection * 2000.0f;
		FCollisionQueryParams collisionParams;
		if (ActorLineTraceSingle(OutHit, rayStart, rayEnd, ECC_WorldStatic, collisionParams)) {
			spawnLocation.Z = OutHit.Location.Z;
		}*/
		
		//orientation
		float angle = FMath::Atan2(pedestrians->velX[i], pedestrians->velY[i]);
		FRotator rotation = { 0.0f, FMath::RadiansToDegrees(angle) - 90.0f, 0.0f };
		FQuat spawnRotation = rotation.Quaternion();
		
		//model
		int model = pedestrians->gender[i] - 1;
		
		//temp vector
		FTransform tempVector;
		tempVector.SetLocation(spawnLocation);
		tempVector.SetRotation(spawnRotation);
		FVector modelScale = { 0.1, 0.1, 0.1 };
		modelScale.Z *= (pedestrians->body_height[i] / 1.6f);
		
		//it is possible to change the walking animation speed of the material.  I'm using the scale X variable to do it.  scaleX = 1.01 (full speed), scaleX = 1.00 (idle)
		//I'm setting the speed to the maximum.  If you want to change it, make sure that scaleX does not exceeds 1.01.
		float speed = 1.0; //speed = pedestrians->speed[i]
		modelScale.X += speed / 100.0f;
		tempVector.SetScale3D(modelScale);
		//spawn instance
		agents[model]->ISMComp->AddInstance(tempVector);
	}
}

void AFloodPedestrianGameModeBase::ClearPedestrians()
{
	UWorld* const world = GetWorld();
	//clear instances
	if (world && pedestrians->total >= 0)
	{
		for (int i = 0; i < NUM_MODELS; i++)
			agents[i]->ISMComp->ClearInstances();
	}
}

void AFloodPedestrianGameModeBase::loadWaterMesh() {
	//get simulation path
	FString directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	// Windows build uses one extra folder level
#if WITH_EDITOR
	FString path = FPaths::Combine(directory, TEXT("../FLAMEGPU-development/examples/FloodPedestrian_SW_stadium/iterations/" + SimulationFolder  + "/map.obj"));
#else
	FString path = FPaths::Combine(directory, TEXT("../../FLAMEGPU-development/examples/FloodPedestrian_SW_stadium/iterations/" + SimulationFolder + "/map.obj"));
#endif

	const TCHAR* simFolder = *SimulationFolder;
	std::ifstream objFile(*path);
	char code;
	FVector vertex;
	int32 vertexIndex;
	while (objFile >> code) {
		if (code == 'v') {
			objFile >> vertex.X >> vertex.Y >> vertex.Z;
			initialVertexPositions.Add(vertex);
			waterVerts.Add(vertex);
		}
		else if (code == 'f') {
			for (int i = 0; i < 3; i++) {
				objFile >> vertexIndex;
				waterTris.Add(vertexIndex - 1);
			}
		}
	}

	waterMesh->waterMeshComponent->ClearAllMeshSections();
	waterMesh->waterMeshComponent->CreateMeshSection(0, waterVerts, waterTris, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void AFloodPedestrianGameModeBase::UpdateWaterLevel()
{
	const int32 nbVertices = waterVerts.Num();
	for (int32 i = 0; i < nbVertices; i++) {
		// Divide floodCell z by 250 to counteract scaling of water plane - differences in units
		waterVerts[i].Z = initialVertexPositions[i].Z + floodCells->z[i] / zScale;
	}

	waterMesh->waterMeshComponent->UpdateMeshSection(0, waterVerts, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());
}