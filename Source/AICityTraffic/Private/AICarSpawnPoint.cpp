// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarSpawnPoint.h"
#include "Components/BoxComponent.h"
#include "AICarSpline.h"

AAICarSpawnPoint::AAICarSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	SphereTraceObject = CreateDefaultSubobject<UBoxComponent>(TEXT("Sphere Trace Obj"));
	SphereTraceObject->SetupAttachment(GetRootComponent());
	SphereTraceObject->SetGenerateOverlapEvents(false);
	SphereTraceObject->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereTraceObject->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	SphereTraceObject->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}