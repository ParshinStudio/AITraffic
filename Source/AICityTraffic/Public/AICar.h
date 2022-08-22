// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "AICar.generated.h"

class UWheeledVehicleMovementComponent4W;
class USkeletalMeshComponent;
class UBoxComponent;

class AAICarSpline;
class USplineComponent;

UCLASS()
class AICITYTRAFFIC_API AAICar : public AWheeledVehicle
{
	GENERATED_BODY()
public:
	AAICar();

	USkeletalMeshComponent* CarMesh;
	UWheeledVehicleMovementComponent4W* AICarMovementComponent;
	USplineComponent* CurrentSplineComponent;

	UPROPERTY(EditAnywhere, Category = "AIConfig|UpdateRate", meta = (ClampMin = 0.0f, ClampMax = 5.0f))
	float UpdateMovementRate = 0.1f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|UpdateRate", meta = (ClampMin = 0.0f, ClampMax = 5.0f))
	float UpdateCollisionTurningRate = 0.1f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|UpdateRate", meta = (ClampMin = 0.0f, ClampMax = 5.0f))
	float UpdateCollisionRate = 0.1f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float XTraceOffset = 210.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float ZTraceOffsetForward = 45.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float ZTraceOffsetTurning = 30.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float TraceWidthForward = 90.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float TraceWidthTurningRight = 120.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float TraceWidthTurningLeft = 200.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float DefaultObjectTraceLenghtRight = 400.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float DefaultObjectTraceLenghtLeft = 2300.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float ObjectTraceLenghtLeftWhenFinishTurn = 400.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace", meta = (ClampMin = 0.0f, ClampMax = 90.0f))
	float TurningTraceOffsetRight = 45.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace", meta = (ClampMin = -90.0f, ClampMax = 0.0f))
	float TurningTraceOffsetLeft = -12.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace", meta = (ClampMin = 0.0f, ClampMax = 90.0f))
	float TurningLeftDeactivateTraceAngle = 20.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float TraceForwardMinLenght = 400.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float TraceForwardMaxLenght = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float ChangeLineTraceLength = 350.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float ChangeLineTraceOffset = 90.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	float ChangeLineTraceWidth = 400.0f;



	UPROPERTY(EditAnywhere, Category = "AIConfig|Trace")
	bool EnableTraceDraw = true;

	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float ThrottleInputMin = 0.4f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float ThrottleInputMax = 0.7f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float EmergencyStopDistance = 300.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties", meta = (ClampMin = -1.0f, ClampMax = 0.0f))
	float ThrottleInputBack = -0.25f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties", meta = (ClampMin = 0.1f))
	float LeaveParkingSlotTimer = 2.0f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float ThrottleToChangeLineToFast = 0.6f;
	UPROPERTY(EditAnywhere, Category = "AIConfig|VehicleProperties")
	float SwitchToOldLineDelay = 6.0f;

	UPROPERTY(EditAnywhere, Category = "AIConfig")
	AAICarSpline* CurrentSpline;
	UPROPERTY(EditAnywhere, Category = "AIConfig")
	TArray<TEnumAsByte<EObjectTypeQuery>> CarCollisionResponse;

	FTimerHandle UpdateMovementHandle;
	FTimerHandle UpdateCollisionHandle;
	FTimerHandle UpdateCollisionOnTurnHandle;

	FTimerHandle EmptyCrossRoadHandle;
	FTimerHandle LeaveParkingSlotHandle;
	FTimerHandle ComeBackToSlowTrackHandle;

	void OverlapTrafficLightBox(bool TriggerBoxState) { CanMove = TriggerBoxState; };
	void ChooseNewSplineOnCrossroad(TArray<AAICarSpline*> NewSplines);
	void SetNewSpline(AAICarSpline* NewSpline);

	void SetIsOnCrossRoad(bool NewIsOnCrossRoad) { IsOnCrossRoad = NewIsOnCrossRoad; };
	void SetStartLeftTurnRotation(FRotator StartRotation) { StartLeftTurnRotation = StartRotation; };

	void MakeLeftTraceShort();
	void SetDefaultLeftTrace();

	void SetDefaultStates();

	void SetGreenLightTimer(float NewGreenLightTimer) { GreenLightTime = NewGreenLightTimer; };
	void SetMainDirection(bool OnMainDirection) { IsOnMainDirection = OnMainDirection; };

	void StopInParkingSlot(bool ParkingIsBoxParking);
	void LeaveParkingSlot();
	void StopLeaveParkingSlot(AAICarSpline* OldSpline);

	bool IsOnMainDirection;
	bool MovingBack;

protected:
	virtual void BeginPlay() override;

private:
	void SetMovementTimers(bool TimersState);
	void UpdateMovement();
	void UpdateCollision();
	void UpdateCollisionOnTurn();
	void UpdateMovementBack();

	void StartMoving();
	void StopMoving();
	void StopMovingEmergency();

	void ChangeLine(AAICarSpline* NewLine, bool OvertakeChange);
	void ChangeLineToSlowTrack();

	float ThrottleInput;

	bool TurningRight;
	bool TurningLeft;

	bool HaveCollisionForward;
	bool HaveSideCollision;

	bool CanMove = true;
	bool Moving;
	bool IsOnCrossRoad;

	bool EmergencyBrake;

	float DefaultThrottleInput;
	float NewSteeringInput;

	float ObjectTraceLenght;
	float TraceWidthTurning;
	FRotator RotatorTurningTraceOffset;
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	bool LeftTraceShort;
	float ObjectTraceLenghtLeft;
	FRotator StartLeftTurnRotation;

	bool ChangingLine;
	bool OtherCarThrottleUsed;
	float SpeedLimit;

	float GreenLightTime;
	
	bool IsInBoxParking;

	AAICarSpline* PreviousSpline;
};