// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICarTrafficLightTwoLines.h"
#include "AICarTrafficLightThreeLines.generated.h"

UCLASS()
class AICITYTRAFFIC_API AAICarTrafficLightThreeLines : public AAICarTrafficLightTwoLines
{
	GENERATED_BODY()
public:
	AAICarTrafficLightThreeLines();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightFourth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightThirdArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FourthGreenLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FourthYellowLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FourthRedLight;

	virtual void SwitchMainLights(ELightState NewLightState) override;
	virtual void SwitchYellowLight(bool Switch) override;
};
