// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICarTrafficLight.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USpotLightComponent;

class AAICarTriggerVolume;
class AAICarPedestrianLight;

UENUM(BlueprintType)
enum class ELightState : uint8
{
	Green = 0 UMETA(DisplayName = "Green"),
	Red = 1	UMETA(DisplayName = "Red"),
	Pedestrians = 3 UMETA(DisplayName = "Pedestrians"),
	Delay = 4 UMETA(DisplayName = "Delay")
};

UCLASS()
class AICITYTRAFFIC_API AAICarTrafficLight : public AActor
{
	GENERATED_BODY()

public:
	AAICarTrafficLight();

	UPROPERTY(EditAnywhere, Category = "AIConfig|TrafficConfig", meta = (EditCondition = "IsInSameDirection == false"))
	bool IsParentLight;
	UPROPERTY(EditAnywhere, Category = "AIConfig|TrafficConfig", meta = (EditCondition = "IsParentLight == false"))
	bool IsInSameDirection;
	UPROPERTY(EditAnywhere, Category = "AIConfig|TrafficConfig")
	bool IsMainDirection;
	UPROPERTY(EditAnywhere, Category = "AIConfig|TrafficConfig", meta = (EditCondition = "IsParentLight == true"))
	TArray<AAICarTrafficLight*> ChildrenLights;
	UPROPERTY(EditAnywhere, Category = "AIConfig|TrafficConfig")
	TArray<AAICarTriggerVolume*> LightTriggerBoxes;
	UPROPERTY(EditAnywhere, Category = "AIConfig|TrafficConfig")
	TArray<AAICarPedestrianLight*> PedestrianLights;

	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 180.0f))
	float GreenLightTime = 10.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 180.0f))
	float RedLightTime = 10.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 180.0f))
	float PedestriansTime = 8.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 10.0f))
	float YellowActiveTimer = 0.35f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 60.0f))
	float DelayAfterGreen = 3.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 60.0f))
	float DelayAfterRed = 3.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Timers", meta = (EditCondition = "IsParentLight == true", ClampMin = 0.0f, ClampMax = 60.0f))
	float DelayAfterPedestrians = 6.0f;

	FTimerHandle YellowLightDelayHandle;
	FTimerHandle MainLightSwitchHandle;
	ELightState LightState = ELightState::Green;
	ELightState PreviousLightState;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshTrafficLightPrimary;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PrimaryGreenLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PrimaryYellowLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PrimaryRedLight;

	void ChangeLightState();

	void SetGreenState();
	void SetRedState();
	void SetPedestriansState();
	void SetDelayState();

	void SetLightCondition(ELightState NewLightState);
	void SetLightConditionResume();

	virtual void SwitchMainLights(ELightState NewLightState);
	virtual void SwitchYellowLight(bool Switch);
	bool YellowLightState = false;	
};