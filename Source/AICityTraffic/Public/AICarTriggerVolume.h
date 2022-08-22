// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICarTriggerVolume.generated.h"

class UBoxComponent;
class AAICar;
class USkeletalMeshComponent;
class AAICarSpline;

UENUM(BlueprintType)
enum class ETriggerType : uint8
{
	TrafficLight = 0	UMETA(DisplayName = "TrafficLight"),
	SwitchLine = 1	UMETA(DisplayName = "SwitchLine"),
	CrossRoad = 2 UMETA(DisplayName = "CrossRoad"),
	ParkingTrigger = 3 UMETA(DisplayName = "ParkingTrigger"),
	ParkingSlot = 4 UMETA(DisplayName = "ParkingSlot"),
	CarDestructor = 5 UMETA(DisplayName = "CarDestructor")
};

UCLASS()
class AICITYTRAFFIC_API AAICarTriggerVolume : public AActor
{
	GENERATED_BODY()

public:
	AAICarTriggerVolume();

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	ETriggerType TriggerType;
	UPROPERTY(EditAnywhere, Category = "AIConfig", meta = (EditCondition = "TriggerType != ETriggerType::CrossRoad && TriggerType != ETriggerType::ParkingSlot"))
	TArray<AAICarSpline*> NewSplines;
	UPROPERTY(EditAnywhere, Category = "AIConfig", meta = (EditCondition = "TriggerType == ETriggerType::ParkingTrigger"))
	bool BoxTriggerParking;
	UPROPERTY(EditAnywhere, Category = "AIConfig", meta = (EditCondition = "TriggerType == ETriggerType::ParkingSlot"))
	bool BoxParking;
	
	FTimerHandle ParallelParkingOccupiedHandle;

	void SetLetPassCrossRoads(bool CanPassCrossRoads);
	void SetGreenLightTimer(float GreenLightTime) { GreenLightTimer = GreenLightTime; };
	void SetIsMainDirection(bool MainDirection) { IsMainDirection = MainDirection; };

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* TriggerVolume;

private:
	UFUNCTION()
	void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	AAICar* TriggeredCar;
	bool LetPassCrossRoads;
	float GreenLightTimer;
	bool IsMainDirection;
	AAICarSpline* OldSpline;

	void ResetOccupied();
	bool Occupied;
};