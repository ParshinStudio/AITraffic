// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICarSpline.generated.h"

class USplineComponent;
class USceneComponent;

UCLASS()
class AICITYTRAFFIC_API AAICarSpline : public AActor
{
	GENERATED_BODY()

public:
	AAICarSpline();

	UPROPERTY(EditAnywhere, Category = "AIConfig")
		AAICarSpline* ParallelTrack;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
		bool FastTrack;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
		float SplineSpeedLimit = 20.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
		bool IsRightTurnSpline;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
		bool IsLeftTurnSpline;

	USplineComponent* GetSplineComponent() { return SplineComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USplineComponent* SplineComponent;
};
