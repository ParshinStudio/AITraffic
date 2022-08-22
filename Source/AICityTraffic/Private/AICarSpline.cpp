// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarSpline.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

AAICarSpline::AAICarSpline()
{
	PrimaryActorTick.bCanEverTick = false;
	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	SetRootComponent(SplineComponent);
	SplineComponent->RegisterComponentWithWorld(GetWorld());
}