// Fill out your copyright notice in the Description page of Project Settings.

#include "AICar.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

#include "AICarSpline.h"
#include "Components/SplineComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

AAICar::AAICar()
{
	PrimaryActorTick.bCanEverTick = false;
	AICarMovementComponent = Cast<UWheeledVehicleMovementComponent4W>(GetVehicleMovementComponent());
	CarMesh = GetMesh();

	CarCollisionResponse.Add(EObjectTypeQuery::ObjectTypeQuery5); // Vehicle
	AActor* SelfIgnore = Cast<AActor>(this);
	ActorsToIgnore.Add(SelfIgnore);
}

void AAICar::BeginPlay()
{
	Super::BeginPlay();
	ObjectTraceLenghtLeft = DefaultObjectTraceLenghtLeft;
	DefaultThrottleInput = FMath::RandRange(ThrottleInputMin, ThrottleInputMax);
	ThrottleInput = DefaultThrottleInput;
	SetMovementTimers(true);

	//USE IF CAR PLACED IN WORLD
	//CurrentSplineComponent = CurrentSpline->GetSplineComponent();
	//SpeedLimit = CurrentSpline->SplineSpeedLimit;
}
void AAICar::SetMovementTimers(bool TimersState)
{
	switch (TimersState)
	{
	case true:
		GetWorldTimerManager().SetTimer(UpdateCollisionHandle, this, &AAICar::UpdateCollision, UpdateCollisionRate, true, 0.1f);
		GetWorldTimerManager().SetTimer(UpdateCollisionOnTurnHandle, this, &AAICar::UpdateCollisionOnTurn, UpdateCollisionTurningRate, true, 0.1f);
		GetWorldTimerManager().SetTimer(UpdateMovementHandle, this, &AAICar::UpdateMovement, UpdateMovementRate, true, 0.1f);
		break;
	case false:
		GetWorldTimerManager().ClearTimer(UpdateMovementHandle);
		GetWorldTimerManager().ClearTimer(UpdateCollisionHandle);
		GetWorldTimerManager().ClearTimer(UpdateCollisionOnTurnHandle);
		break;
	}
}
void AAICar::UpdateMovement()
{
	if (!HaveCollisionForward && !HaveSideCollision && CanMove)
	{
		if (!Moving)
		{
			StartMoving();
		}

		// Calculate Steering Input
		FVector CarLocation = CarMesh->GetSocketLocation(FName("AxleLocation"));
		FVector TangentClosest = CurrentSplineComponent->FindTangentClosestToWorldLocation(CarLocation, ESplineCoordinateSpace::World);
		FVector	LocationClosest = CurrentSplineComponent->FindLocationClosestToWorldLocation(TangentClosest.GetSafeNormal(0.0001f) * 100.0f + CarLocation, ESplineCoordinateSpace::World);
		FRotator FindLookAtRotation = UKismetMathLibrary::FindLookAtRotation(CarLocation, LocationClosest);
		FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(FindLookAtRotation, GetActorRotation());
		NewSteeringInput = FMath::GetMappedRangeValueClamped(FVector2D(-90.0f, 90.0f), FVector2D(-1.0f, 1.0f), DeltaRotator.Yaw);

		// Limitation and set Steering Input depend on ChangingLine state
		if (!ChangingLine)
		{
			AICarMovementComponent->SetSteeringInput(NewSteeringInput);
		}
		else
		{
			AICarMovementComponent->SetSteeringInput(FMath::Clamp(NewSteeringInput, -0.1f, 0.1f));
		}
		// Update Throttle Input depend on turning radius
		if (!ChangingLine && !OtherCarThrottleUsed && GetVelocity().Size() * 0.036f < SpeedLimit - 1.0f)
		{
			AICarMovementComponent->SetThrottleInput(FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(DefaultThrottleInput, 0.3), FMath::Abs(NewSteeringInput)));
		}
		// Use Throttle Input of other car
		else if (!ChangingLine && OtherCarThrottleUsed && GetVelocity().Size() * 0.036f < SpeedLimit - 1.0f)
		{
			AICarMovementComponent->SetThrottleInput(ThrottleInput);
		}
		// Allow Car increase speed when ChangingLine
		else if (ChangingLine && GetVelocity().Size() * 0.036f < SpeedLimit - 1.0f)
		{
			AICarMovementComponent->SetThrottleInput(DefaultThrottleInput);
		}
		// Make car keep speed in speed limit range
		else if (GetVelocity().Size() * 0.036f >= SpeedLimit - 1.0f)
		{
			AICarMovementComponent->SetThrottleInput(0.0f);
		}
	}
	// Stop or use 0.0 throttle depend on case
	else if ((HaveCollisionForward || HaveSideCollision || !CanMove) && !EmergencyBrake)
	{
		if (HitResult.Distance < EmergencyStopDistance || IsOnCrossRoad || !CanMove)
		{
			StopMovingEmergency();
		}
		else 
		{
			StopMoving();
		}
	}
}
void AAICar::UpdateCollision()
{
	if (IsOnCrossRoad && (TurningRight || TurningLeft))
	{
		if (!HaveCollisionForward) return;
		HaveCollisionForward = false;
		return;
	}
	if (CanMove)
	{
		// Calc forward trace
		FRotator TraceRotation = GetActorRotation() + FRotator(0.0f, FMath::Clamp(NewSteeringInput * 25.0f, -25.0f, 25.0f), 0.0f);
		FVector TraceStart = GetActorLocation() + UKismetMathLibrary::GetForwardVector(TraceRotation) * XTraceOffset + FVector(0.0f, 0.0f, ZTraceOffsetForward);
		FVector TraceEnd = TraceStart + UKismetMathLibrary::GetForwardVector(TraceRotation) * FMath::Clamp(GetVelocity().Size(), TraceForwardMinLenght, TraceForwardMaxLenght);
		UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), TraceStart, TraceEnd, FVector(0.0, TraceWidthForward, 5.0f), TraceRotation,
		CarCollisionResponse, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, EnableTraceDraw, FLinearColor::Green, FLinearColor::Red, UpdateMovementRate);

		// Overtake
		if (HitResult.bBlockingHit && !ChangingLine && !CurrentSpline->FastTrack && CurrentSpline->ParallelTrack && !IsOnCrossRoad && HitResult.Actor->GetVelocity().Size() + 10.0f < GetVelocity().Size() && HitResult.Actor->GetVelocity().Size() > 50.0f && FMath::Abs(NewSteeringInput) < 0.05)
		{
			HaveCollisionForward = HitResult.bBlockingHit;
			ChangeLine(CurrentSpline->ParallelTrack, true);
		}
		// Change line to fast line
		else if (!ChangingLine && !CurrentSpline->FastTrack && CurrentSpline->ParallelTrack && !IsOnCrossRoad && CurrentSpline->ParallelTrack->SplineSpeedLimit > CurrentSpline->SplineSpeedLimit && DefaultThrottleInput > ThrottleToChangeLineToFast && FMath::Abs(NewSteeringInput) < 0.05)
		{
			HaveCollisionForward = HitResult.bBlockingHit;
			ChangeLine(CurrentSpline->ParallelTrack, false);
		}
		// Check collision in case overtake
		else if (ChangingLine)
		{
			FVector SplineNearestLocation = CurrentSplineComponent->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
			if (FVector::Dist(SplineNearestLocation, GetActorLocation()) < 50.0f)
			{
				HaveCollisionForward = HitResult.bBlockingHit;
				ChangingLine = false;
			}
			else
			{
				if (HitResult.bBlockingHit && HitResult.Distance < EmergencyStopDistance)
				{
					HaveCollisionForward = HitResult.bBlockingHit;

				}
				else
				{
					HaveCollisionForward = false;
				}
			}
		}
		// Take other car throttle if overtake not possible
		else if (!OtherCarThrottleUsed && HitResult.bBlockingHit && !ChangingLine && (CurrentSpline->FastTrack || !CurrentSpline->ParallelTrack) && HitResult.Actor->GetVelocity().Size() + 10.0f < GetVelocity().Size())
		{
			auto SlowAICar = Cast<AAICar>(HitResult.Actor);
			if (SlowAICar)
			{
				ThrottleInput = SlowAICar->DefaultThrottleInput;
				OtherCarThrottleUsed = true;
				HaveCollisionForward = HitResult.bBlockingHit;
			}
		}
		else
		{
			HaveCollisionForward = HitResult.bBlockingHit;
		}
	}
	else
	{
		if (!HaveCollisionForward) return;
		HaveCollisionForward = false;
	}
}
void AAICar::UpdateCollisionOnTurn()
{
	if (IsOnCrossRoad && (TurningRight || TurningLeft) && CanMove)
	{
		if (TurningRight)
		{
			RotatorTurningTraceOffset = FRotator(0.0f, TurningTraceOffsetRight, 0.0f);
			TraceWidthTurning = TraceWidthTurningRight;
			ObjectTraceLenght = DefaultObjectTraceLenghtRight;
		}
		else if (TurningLeft)
		{
			RotatorTurningTraceOffset = FRotator(0.0f, TurningTraceOffsetLeft, 0.0f);
			TraceWidthTurning = TraceWidthTurningLeft;
			ObjectTraceLenght = ObjectTraceLenghtLeft;
		}
		// Calc side trace
		FVector TraceStart = GetActorLocation() + UKismetMathLibrary::GetForwardVector(GetActorRotation()) * XTraceOffset + FVector(0.0f, 0.0f, ZTraceOffsetTurning);
		FRotator TraceRotation = GetActorRotation() + RotatorTurningTraceOffset;
		FVector TraceEnd = TraceStart + UKismetMathLibrary::GetForwardVector(TraceRotation) * ObjectTraceLenght;
		UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), TraceStart, TraceEnd, FVector(0.0, TraceWidthTurning, 5.0f), TraceRotation,
		CarCollisionResponse, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, EnableTraceDraw, FLinearColor::Green, FLinearColor::Red, UpdateMovementRate);

		HaveSideCollision = HitResult.bBlockingHit;

		// Make trace short if stay long time on crossroad or turning angle on start manuevere more than TurningLeftDeactivateTraceAngle
		if (!GetWorld()->GetTimerManager().IsTimerActive(EmptyCrossRoadHandle) && TurningLeft && !LeftTraceShort && HaveSideCollision && IsOnCrossRoad)
		{
			GetWorldTimerManager().SetTimer(EmptyCrossRoadHandle, this, &AAICar::MakeLeftTraceShort, 1.0, false, GreenLightTime);
		}
		else if (FMath::Abs(StartLeftTurnRotation.Yaw - GetActorRotation().Yaw) > TurningLeftDeactivateTraceAngle && TurningLeft && !LeftTraceShort && IsOnCrossRoad )
		{
			MakeLeftTraceShort();
		}

		// Ignore other car if other car is not on main road and turning left
		if (TurningLeft && HitResult.bBlockingHit && IsOnMainDirection)
		{
			auto AICar = Cast<AAICar>(HitResult.Actor);
			if (AICar && !AICar->IsOnMainDirection && AICar->TurningLeft)
			{
				HaveSideCollision = false;
			}
		}
	}
	else
	{
		if (!HaveSideCollision) return;
		HaveSideCollision = false;
	}
}

// Movement States
void AAICar::StartMoving()
{
	AICarMovementComponent->SetHandbrakeInput(false);
	AICarMovementComponent->SetThrottleInput(ThrottleInput);
	EmergencyBrake = false;
	Moving = true;
}
void AAICar::StopMoving()
{
	AICarMovementComponent->SetThrottleInput(0.0f);
	Moving = false;
}
void AAICar::StopMovingEmergency()
{
	AICarMovementComponent->SetThrottleInput(0.0f);
	AICarMovementComponent->SetHandbrakeInput(true);
	Moving = false;
	EmergencyBrake = true;
}

void AAICar::MakeLeftTraceShort()
{
	if (!IsOnCrossRoad) return;
	LeftTraceShort = true;
	ObjectTraceLenghtLeft = ObjectTraceLenghtLeftWhenFinishTurn;
}
void AAICar::SetDefaultLeftTrace()
{
	LeftTraceShort = false;
	ObjectTraceLenghtLeft = DefaultObjectTraceLenghtLeft;
	GetWorldTimerManager().ClearTimer(EmptyCrossRoadHandle);
}
void AAICar::SetDefaultStates()
{
	TurningRight = false;
	TurningLeft = false;
	ThrottleInput = DefaultThrottleInput;
	OtherCarThrottleUsed = false;
}

// Change line and overtake
void AAICar::ChooseNewSplineOnCrossroad(TArray<AAICarSpline*> NewSplines)
{
	int32 NewSplineIndex = FMath::RandRange(0, NewSplines.Num() - 1);
	CurrentSpline = NewSplines[NewSplineIndex];
	CurrentSplineComponent = CurrentSpline->GetSplineComponent();
	SpeedLimit = CurrentSpline->SplineSpeedLimit;
	if (CurrentSpline->IsRightTurnSpline)
	{
		TurningRight = true;
	}
	else if (CurrentSpline->IsLeftTurnSpline)
	{
		TurningLeft = true;
	}
}
void AAICar::SetNewSpline(AAICarSpline* NewSpline)
{
	CurrentSpline = NewSpline;
	CurrentSplineComponent = CurrentSpline->GetSplineComponent();
	SpeedLimit = CurrentSpline->SplineSpeedLimit;
}
void AAICar::ChangeLine(AAICarSpline* NewLine, bool OvertakeChange)
{
	// Calc direction of trace check
	FRotator LookAtSpline = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NewLine->GetSplineComponent()->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World));
	FVector LookAtSplineFV = UKismetMathLibrary::GetForwardVector(LookAtSpline);
	float AngleInDeg = UKismetMathLibrary::DegAtan2(GetActorForwardVector().Y, GetActorForwardVector().X) - UKismetMathLibrary::DegAtan2(LookAtSplineFV.Y, LookAtSplineFV.X);
	float TraceDirectionMultiply;
	AngleInDeg > 0 ? TraceDirectionMultiply = -1.0f : TraceDirectionMultiply = 1.0f;

	// Make trace result when car try change line
	FHitResult OvertakeHitResult;
	FRotator TraceRotation = GetActorRotation() + FRotator(0.0f, 90.0f, 0.0f);
	FVector TraceStart = GetActorLocation() + GetActorRightVector() * TraceDirectionMultiply * ChangeLineTraceOffset + FVector(0.0f, 0.0f, ZTraceOffsetTurning);
	FVector TraceEnd = TraceStart + GetActorRightVector() * TraceDirectionMultiply * ChangeLineTraceWidth;
	UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), TraceStart, TraceEnd, FVector(0.0, ChangeLineTraceLength, 5.0f), TraceRotation,
	CarCollisionResponse, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OvertakeHitResult, EnableTraceDraw, FLinearColor::Green, FLinearColor::Red, UpdateMovementRate);

	if (!OvertakeHitResult.bBlockingHit)
	{
		if (!CurrentSpline->FastTrack)
		{
			PreviousSpline = CurrentSpline;
		}
		ChangingLine = true;
		SetNewSpline(NewLine);

		// Start come back to previous line after overtaking
		if (CurrentSpline->FastTrack && !GetWorldTimerManager().IsTimerActive(ComeBackToSlowTrackHandle) && OvertakeChange)
		{
			GetWorldTimerManager().SetTimer(ComeBackToSlowTrackHandle, this, &AAICar::ChangeLineToSlowTrack, UpdateMovementRate, true, SwitchToOldLineDelay);
		}
		else if (!CurrentSpline->FastTrack && GetWorldTimerManager().IsTimerActive(ComeBackToSlowTrackHandle) && OvertakeChange)
		{
			GetWorldTimerManager().ClearTimer(ComeBackToSlowTrackHandle);
		}
	}
}
void AAICar::ChangeLineToSlowTrack()
{
	ChangeLine(PreviousSpline, false);
}


// Parking
void AAICar::StopInParkingSlot(bool IsBoxParking)
{
	IsInBoxParking = IsBoxParking;
	SetMovementTimers(false);
	StopMovingEmergency();
	CarMesh->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	GetWorldTimerManager().SetTimer(LeaveParkingSlotHandle, this, &AAICar::LeaveParkingSlot, 1.0f, false, LeaveParkingSlotTimer);
}
void AAICar::LeaveParkingSlot()
{
	CarMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	if (IsInBoxParking)
	{
		MovingBack = true;
		GetWorldTimerManager().SetTimer(LeaveParkingSlotHandle, this, &AAICar::UpdateMovementBack, UpdateMovementRate, true, 0.1f);
		AICarMovementComponent->SetHandbrakeInput(false);
		AICarMovementComponent->SetThrottleInput(ThrottleInputBack);
		EmergencyBrake = false;
	}
	else
	{
		SetMovementTimers(true);
	}
}
void AAICar::StopLeaveParkingSlot(AAICarSpline* OldSpline)
{
	GetWorldTimerManager().ClearTimer(LeaveParkingSlotHandle);
	StopMovingEmergency();
	MovingBack = false;
	SetNewSpline(OldSpline);
	SetMovementTimers(true);
}

void AAICar::UpdateMovementBack()
{
	// Calculate steering when leave box parking
	FVector CarLocation = CarMesh->GetSocketLocation(FName("AxleLocationBack"));
	FVector TangentClosest = CurrentSplineComponent->FindTangentClosestToWorldLocation(CarLocation, ESplineCoordinateSpace::World);
	FVector	LocationClosest = CurrentSplineComponent->FindLocationClosestToWorldLocation(TangentClosest.GetSafeNormal(0.0001f) * -100.0f + CarLocation, ESplineCoordinateSpace::World);
	FRotator FindLookAtRotation = UKismetMathLibrary::FindLookAtRotation(CarLocation, LocationClosest);
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(FindLookAtRotation, GetActorRotation() - FRotator(0.0f, 180.0f, 0.0f));
	NewSteeringInput = FMath::GetMappedRangeValueClamped(FVector2D(-90.0f, 90.0f), FVector2D(-1.0f, 1.0f), DeltaRotator.Yaw);
	AICarMovementComponent->SetSteeringInput(NewSteeringInput * -1.0f);

	// Make trace result when leave box parking
	FHitResult MoveBackHitResult;
	FRotator TraceRotation = GetActorRotation() + FRotator(0.0f, 180.0f, 0.0f);
	FVector TraceStart = CarMesh->GetSocketLocation(FName("AxleLocationBack")) + UKismetMathLibrary::GetForwardVector(TraceRotation) * 20.0f + FVector(0.0f, 0.0f, ZTraceOffsetForward);
	FVector TraceEnd = TraceStart + UKismetMathLibrary::GetForwardVector(TraceRotation) * 400.0f;
	UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), TraceStart, TraceEnd, FVector(0.0, 100, 5.0f), TraceRotation,
	CarCollisionResponse, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, MoveBackHitResult, EnableTraceDraw, FLinearColor::Green, FLinearColor::Red, UpdateMovementRate);

	if (MoveBackHitResult.bBlockingHit)
	{
		AICarMovementComponent->SetThrottleInput(0.0f);
		AICarMovementComponent->SetHandbrakeInput(true);
	}
	else
	{
		AICarMovementComponent->SetHandbrakeInput(false);
		AICarMovementComponent->SetThrottleInput(ThrottleInputBack);
	}
}