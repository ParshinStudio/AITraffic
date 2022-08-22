// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarTriggerVolume.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "AICar.h"
#include "AICarSpline.h"
#include "Components/SkeletalMeshComponent.h"

AAICarTriggerVolume::AAICarTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("Collision Mesh");
	SetRootComponent(TriggerVolume);
}

void AAICarTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AAICarTriggerVolume::OnBoxOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AAICarTriggerVolume::OnBoxOverlapEnd);
}

void AAICarTriggerVolume::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto SomeAICar = Cast<AAICar>(OtherActor);
	if (!SomeAICar) return;
	switch (TriggerType)
	{
	case ETriggerType::TrafficLight:
		TriggeredCar = SomeAICar;
		TriggeredCar->OverlapTrafficLightBox(LetPassCrossRoads);
		TriggeredCar->ChooseNewSplineOnCrossroad(NewSplines);
		TriggeredCar->SetGreenLightTimer(GreenLightTimer);
		TriggeredCar->SetMainDirection(IsMainDirection);
		if (!LetPassCrossRoads) return;
		TriggeredCar = nullptr;
		break;
	case ETriggerType::SwitchLine:
		SomeAICar->SetNewSpline(NewSplines[0]);
		break;
	case ETriggerType::CrossRoad:
		SomeAICar->SetIsOnCrossRoad(true);
		SomeAICar->SetStartLeftTurnRotation(SomeAICar->GetActorRotation());
		break;
	case ETriggerType::ParkingTrigger:
	{
		int32 RandRange = FMath::RandRange(0, 100);

		if (BoxTriggerParking && !SomeAICar->MovingBack && !Occupied && RandRange < 20)
		{
			OldSpline = SomeAICar->CurrentSpline;
			SomeAICar->SetNewSpline(NewSplines[0]);
			Occupied = true;
		}
		else if (BoxTriggerParking && SomeAICar->MovingBack && Occupied)
		{
			SomeAICar->StopLeaveParkingSlot(OldSpline);
			Occupied = false;
		}
		else if (!BoxTriggerParking && !Occupied &&  RandRange < 20)
		{
			SomeAICar->SetNewSpline(NewSplines[0]);
			Occupied = true;
			GetWorldTimerManager().SetTimer(ParallelParkingOccupiedHandle, this, &AAICarTriggerVolume::ResetOccupied, 1.0f, false, SomeAICar->LeaveParkingSlotTimer + 5.0f);
		}
		break;
	}
	case ETriggerType::ParkingSlot:
		if (BoxParking)
		{
			SomeAICar->StopInParkingSlot(true);
		}
		else
		{
			SomeAICar->StopInParkingSlot(false);
		}
		break;
	case ETriggerType::CarDestructor:
		SomeAICar->Destroy();
		break;
	}
}
void AAICarTriggerVolume::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (TriggerType == ETriggerType::TrafficLight || TriggerType == ETriggerType::CrossRoad)
	{
		auto SomeAICar = Cast<AAICar>(OtherActor);
		if (!SomeAICar) return;
		switch (TriggerType)
		{
		case ETriggerType::TrafficLight:
			TriggeredCar = nullptr;
			break;
		case ETriggerType::CrossRoad:
			SomeAICar->SetIsOnCrossRoad(false);
			SomeAICar->SetDefaultLeftTrace();
			SomeAICar->SetMainDirection(false);
			SomeAICar->SetDefaultStates();
			break;
		}
	}
}
void AAICarTriggerVolume::SetLetPassCrossRoads(bool CanPassCrossRoads)
{
	LetPassCrossRoads = CanPassCrossRoads;
	if (!TriggeredCar) return;
	TriggeredCar->OverlapTrafficLightBox(CanPassCrossRoads);
	TriggeredCar->SetDefaultLeftTrace();
}
void AAICarTriggerVolume::ResetOccupied()
{
	Occupied = false;
}