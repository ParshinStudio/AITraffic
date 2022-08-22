// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICarTrafficLightWithArm.h"
#include "AICarTrafficLightTwoLines.generated.h"

UCLASS()
class AICITYTRAFFIC_API AAICarTrafficLightTwoLines : public AAICarTrafficLightWithArm
{
	GENERATED_BODY()
public:
	AAICarTrafficLightTwoLines();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightThird;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightSecondArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ThirdGreenLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ThirdYellowLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ThirdRedLight;

	virtual void SwitchMainLights(ELightState NewLightState) override;
	virtual void SwitchYellowLight(bool Switch) override;
};