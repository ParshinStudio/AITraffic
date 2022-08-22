// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

class AAICar;

UCLASS()
class AICITYTRAFFIC_API AAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AAISpawner();
	
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	bool SpawnCars = true;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	TSubclassOf<AAICar> AICarToSpawn;

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	int32 MinCarsAround = 5;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	int32 MaxCarsAround = 15;

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	float MinSpawnDistance = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	float TraceBlockSpawnDistance = 5000.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	float MaxSpawnDistance = 10000.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	float DestructorDistance = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	float CarSpawnRate = 5.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceCollisionResponse;

protected:
	virtual void BeginPlay() override;

	void CarsSpawner();
	FTimerHandle CarSpawnerHandle;
};