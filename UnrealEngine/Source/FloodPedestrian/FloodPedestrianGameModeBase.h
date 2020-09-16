// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Windows/MinWindows.h"
#include "Pedestrian.h"
#include "WaterMesh.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/TextBlock.h"
#include "FloodPedestrianGameModeBase.generated.h"



#define xmachine_memory_agent_MAX 16384 //same number of agents as FLAME
#define GRID_SIZE 128
#define NUM_MODELS 2 //corresponds to gender variable (agent model)
#define SCALE 250.0f

//agent data read from FLAME
struct pedestrian_data
{
	int total;
	int gender[xmachine_memory_agent_MAX];
	float x[xmachine_memory_agent_MAX];
	float y[xmachine_memory_agent_MAX];
	float z[xmachine_memory_agent_MAX];
	float velX[xmachine_memory_agent_MAX];
	float velY[xmachine_memory_agent_MAX];
	float speed[xmachine_memory_agent_MAX];
	float body_height[xmachine_memory_agent_MAX];
};

//water data read from FLAME
struct flood_data
{
	int total;
	float x[xmachine_memory_agent_MAX];
	float y[xmachine_memory_agent_MAX];
	float z[xmachine_memory_agent_MAX];
};

// Holds simulation state data from FLAME
struct simulation_state_data
{
	int new_sim_time;
	int evacuated_population;
	int awaiting_evacuation_population;

	int count_in_dry;
	int count_at_low_risk;
	int count_at_medium_risk;
	int count_at_high_risk;
	int count_at_highest_risk;
};

//simulation options sent to FLAME
struct options_data
{
	int lock;
	int quit;
};

/**
 * 
 */
UCLASS()
class FLOODPEDESTRIAN_API AFloodPedestrianGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFloodPedestrianGameModeBase();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

protected:

	//HUD functions
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartSimulation();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StopSimulation();

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void Quit();

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void UpdateFolderPath(FString newPath);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void setZScale(float newZScale);
	//HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> menuWidgetClass;
	UPROPERTY()
		class UUserWidget* menuWidget;

private:

	bool simulationStarted;
	bool firstFrame; //used to store the intial vertex positions of the water plane

	TArray<FVector> waterVerts;
	TArray<int32> waterTris;
	void loadWaterMesh();
	void SpawnTopographyAndWater();

	float zScale = 250.0f;
	

	TArray<FVector> initialVertexPositions;
	TArray<FVector> pedPositions;

	//communication
	FProcHandle handle;
	HANDLE hPedestrianMapFile, hFloodMapFile, hSimulationStateMapFile, hOptionsMapFile;
	LPCTSTR pedestrianBuf, floodBuf, simulationStateBuf, optionsBuf;
	pedestrian_data* pedestrians;
	flood_data* floodCells;
	simulation_state_data* simulationStateData;
	options_data* options;

	UPROPERTY(EditAnyWhere, Category = "Configuration")
		FString SimulationFolder;

	//map, this is the plane to be modified by the water level
	//UPROPERTY(EditAnyWhere, Category = "Configuration")
		UStaticMesh* SMMap;
		UStaticMesh* TopographyMesh;
    
	
	AStaticMeshActor* topography;
	AStaticMeshActor* map;
	AWaterMesh* waterMesh;
	
	//use this if you want to add material to map
	/*UPROPERTY(EditAnyWhere, Category = "Maps")
		UMaterial* waterMaterial;*/

	//store previous displacement to only add the difference each frame
	TArray<float> previousDisplacement;

	//pedestrians
	TArray<APedestrian*> agents;

	

	//Character 0
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UStaticMesh* SMPedestrian0;
	//LOD0
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* pedestrianMaterial00;
	//LOD1
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* pedestrianMaterialLOD100;
	//LOD2
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* pedestrianMaterialLOD200;


	//Character 1
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UStaticMesh* SMPedestrian1;
	//LOD0
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* pedestrianMaterial10;
	//LOD1
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* pedestrianMaterialLOD110;
	//LOD2
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* pedestrianMaterialLOD210;

	//wigs
	//Character 1
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* wigMaterial10;
	//LOD1
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* wigMaterialLOD110;
	//LOD2
	UPROPERTY(EditAnyWhere, Category = "Characters")
		UMaterial* wigMaterialLOD210;
		
	// Water material for water plane
	UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterial* waterMaterial;

	//lists of materials
	TArray<UMaterial*> materials;
	TArray<UMaterial*> materialsLOD1;
	TArray<UMaterial*> materialsLOD2;
	TArray<UMaterial*> wigs;
	TArray<UMaterial*> wigsLOD1;
	TArray<UMaterial*> wigsLOD2;


	void Communication();
	void SpawnPedestrians();
	void ClearPedestrians();
	void UpdateWaterLevel();
	void UpdateHUD();
	
};
