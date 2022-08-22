// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICarTrafficLight.h"
#include "AICarTrafficLightWithArm.generated.h"

UCLASS()
class AICITYTRAFFIC_API AAICarTrafficLightWithArm : public AAICarTrafficLight
{
	GENERATED_BODY()

public:
	AAICarTrafficLightWithArm();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightSecondary;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SecondaryGreenLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SecondaryYellowLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SecondaryRedLight;

	virtual void SwitchMainLights(ELightState NewLightState) override;
	virtual void SwitchYellowLight(bool Switch) override;
};
