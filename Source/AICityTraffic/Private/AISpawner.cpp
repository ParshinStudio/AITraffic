// Fill out your copyright notice in the Description page of Project Settings.

#include "AISpawner.h"

#include "AICarSpawnPoint.h"
#include "AICar.h"
#include "AICarSpline.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAISpawner::AAISpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}
void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
	CarsSpawner();
	GetWorldTimerManager().SetTimer(CarSpawnerHandle, this, &AAISpawner::CarsSpawner, CarSpawnRate, true, 2.0);
}

void AAISpawner::CarsSpawner()
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn()->GetActorLocation();

	TArray<AActor*> FoundCars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAICar::StaticClass(), FoundCars);
	for (int i = 0; i < FoundCars.Num(); i++)
	{
		float Distance = FVector::Dist(PlayerLocation, FoundCars[i]->GetActorLocation());
		if (Distance > DestructorDistance)
		{
			FoundCars[i]->Destroy();
		}
	}
	FoundCars.Shrink();

	int32 NewCarsAround = FMath::RandRange(MinCarsAround, MaxCarsAround);
	if (NewCarsAround > FoundCars.Num())
	{
		int32 CarsToSpawn = NewCarsAround - FoundCars.Num();

		TArray<AActor*> IgnoreActors;
		TArray<AActor*> FoundCarSpawnPoints;
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), PlayerLocation, MaxSpawnDistance, TraceCollisionResponse, AAICarSpawnPoint::StaticClass(), IgnoreActors, FoundCarSpawnPoints);
		if (FoundCarSpawnPoints.Num() == 0) { return; }

		TArray<AActor*> NearbyCarSpawnPoints;
		for (int i = 0; i < FoundCarSpawnPoints.Num(); i++)
		{
			float Distance = FVector::Dist(PlayerLocation, FoundCarSpawnPoints[i]->GetActorLocation());
			if (Distance > MinSpawnDistance && Distance < TraceBlockSpawnDistance)
			{
				
				FHitResult HitLineTraceResult;
				GetWorld()->LineTraceSingleByChannel(HitLineTraceResult, PlayerLocation, FoundCarSpawnPoints[i]->GetActorLocation(), ECollisionChannel::ECC_Visibility);
				auto TracedActor = Cast<AAICarSpawnPoint>(HitLineTraceResult.Actor);
				if (!TracedActor)
				{
					NearbyCarSpawnPoints.Add(FoundCarSpawnPoints[i]);
				}
			}
			else if(Distance > TraceBlockSpawnDistance)
			{
				NearbyCarSpawnPoints.Add(FoundCarSpawnPoints[i]);
			}
		}
		if (NearbyCarSpawnPoints.Num() == 0) { return; }

		if (CarsToSpawn > NearbyCarSpawnPoints.Num())
		{
			CarsToSpawn = NearbyCarSpawnPoints.Num();
		}

		TArray<AActor*> UsedCarSpawnPoints;
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		for (int i = 0; i < CarsToSpawn; i++)
		{
			AActor* CarSpawnPoint;
			do
			{
				CarSpawnPoint = NearbyCarSpawnPoints[FMath::RandRange(0, NearbyCarSpawnPoints.Num() - 1)];
			} while (CarSpawnPoint == nullptr || UsedCarSpawnPoints.Contains(CarSpawnPoint));
			UsedCarSpawnPoints.Add(CarSpawnPoint);

			AAICarSpawnPoint* CastedCarSpawnPoint = Cast<AAICarSpawnPoint>(CarSpawnPoint);
			if (CastedCarSpawnPoint && CastedCarSpawnPoint->SpawnSpline)
			{
				auto SplineComponent = CastedCarSpawnPoint->SpawnSpline->GetSplineComponent();
				FVector SpawnLocation = SplineComponent->FindLocationClosestToWorldLocation(CastedCarSpawnPoint->GetActorLocation(), ESplineCoordinateSpace::World);
				FRotator SpawnRotation = SplineComponent->FindRotationClosestToWorldLocation(SpawnLocation, ESplineCoordinateSpace::World);
				AAICar* SpawnedCar = GetWorld()->SpawnActor<AAICar>(AICarToSpawn, SpawnLocation + FVector(0.0f, 0.0f, 3.0f), SpawnRotation, SpawnParams);
				if (SpawnedCar)
				{
					SpawnedCar->SetNewSpline(CastedCarSpawnPoint->SpawnSpline);
				}
			}
		}
	}
}