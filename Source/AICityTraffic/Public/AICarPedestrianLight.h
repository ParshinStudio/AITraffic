// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICarPedestrianLight.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;

UCLASS()
class AICITYTRAFFIC_API AAICarPedestrianLight : public AActor
{
	GENERATED_BODY()

public:
	AAICarPedestrianLight();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshBody;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshNoWalk;

	void SetPedestrianLightState(bool NewState);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* _RootComponent;
};