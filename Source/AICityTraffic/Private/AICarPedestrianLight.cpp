// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarPedestrianLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"

AAICarPedestrianLight::AAICarPedestrianLight()
{
	PrimaryActorTick.bCanEverTick = false;
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	RootComponent = _RootComponent;
	RootComponent->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
	StaticMeshBody = CreateDefaultSubobject<UStaticMeshComponent>("Main Body");
	StaticMeshBody->SetupAttachment(RootComponent);
	StaticMeshBody->SetGenerateOverlapEvents(false);
	StaticMeshWalk = CreateDefaultSubobject<UStaticMeshComponent>("Can Walk");
	StaticMeshWalk->SetupAttachment(RootComponent);
	StaticMeshWalk->SetVisibility(false);
	StaticMeshWalk->SetRelativeLocation(FVector(12.0f, 41.0f, 0.0f));
	StaticMeshWalk->SetGenerateOverlapEvents(false);
	StaticMeshNoWalk = CreateDefaultSubobject<UStaticMeshComponent>("No Walk");
	StaticMeshNoWalk->SetupAttachment(RootComponent);
	StaticMeshNoWalk->SetVisibility(false);
	StaticMeshNoWalk->SetRelativeLocation(FVector(-15.0f, 41.0f, 0.0f));
	StaticMeshNoWalk->SetGenerateOverlapEvents(false);
}
void AAICarPedestrianLight::BeginPlay()
{
	Super::BeginPlay();
}
void AAICarPedestrianLight::SetPedestrianLightState(bool NewState)
{
	switch (NewState)
	{
	case true:
		StaticMeshWalk->SetVisibility(true);
		StaticMeshNoWalk->SetVisibility(false);
		break;
	case false:
		StaticMeshWalk->SetVisibility(false);
		StaticMeshNoWalk->SetVisibility(true);
		break;
	}
}