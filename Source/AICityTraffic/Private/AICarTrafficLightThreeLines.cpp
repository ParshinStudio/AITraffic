// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarTrafficLightThreeLines.h"

AAICarTrafficLightThreeLines::AAICarTrafficLightThreeLines()
{
	StaticMeshTrafficLightThirdArm = CreateDefaultSubobject<UStaticMeshComponent>("ThirdArm");
	StaticMeshTrafficLightThirdArm->SetupAttachment(GetRootComponent());

	StaticMeshTrafficLightFourth = CreateDefaultSubobject<UStaticMeshComponent>("Fourth");
	StaticMeshTrafficLightFourth->SetupAttachment(StaticMeshTrafficLightBody);
	FourthGreenLight = CreateDefaultSubobject<UStaticMeshComponent>("Fourth Green Light");
	FourthGreenLight->SetupAttachment(StaticMeshTrafficLightFourth);
	FourthGreenLight->SetVisibility(false);
	FourthYellowLight = CreateDefaultSubobject<UStaticMeshComponent>("Fourth Yellow Light");
	FourthYellowLight->SetupAttachment(StaticMeshTrafficLightFourth);
	FourthYellowLight->SetVisibility(false);
	FourthRedLight = CreateDefaultSubobject<UStaticMeshComponent>("Fourth Red Light");
	FourthRedLight->SetupAttachment(StaticMeshTrafficLightFourth);
}

void AAICarTrafficLightThreeLines::SwitchMainLights(ELightState NewLightState)
{
	Super::SwitchMainLights(NewLightState);

	switch (NewLightState)
	{
	case ELightState::Red:
	case ELightState::Pedestrians:
	case ELightState::Delay:
		FourthRedLight->SetVisibility(true);
		FourthGreenLight->SetVisibility(false);
		break;

	case ELightState::Green:
		FourthRedLight->SetVisibility(false);
		FourthGreenLight->SetVisibility(true);
		break;
	}
}
void AAICarTrafficLightThreeLines::SwitchYellowLight(bool Switch)
{
	Super::SwitchYellowLight(Switch);
	switch (Switch)
	{
	case  true:
		FourthYellowLight->SetVisibility(true);
		break;

	case false:
		FourthYellowLight->SetVisibility(false);
		break;
	}
}