// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AICarSpawnPoint.generated.h"

class AAICarSpline;
class UBoxComponent;

UCLASS()
class AICITYTRAFFIC_API AAICarSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	AAICarSpawnPoint();

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	AAICarSpline* SpawnSpline;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* SphereTraceObject;
};