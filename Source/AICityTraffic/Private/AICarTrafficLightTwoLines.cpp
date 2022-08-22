// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarTrafficLightTwoLines.h"

AAICarTrafficLightTwoLines::AAICarTrafficLightTwoLines()
{
	StaticMeshTrafficLightSecondArm = CreateDefaultSubobject<UStaticMeshComponent>("SecondArm");
	StaticMeshTrafficLightSecondArm->SetupAttachment(GetRootComponent());

	StaticMeshTrafficLightThird = CreateDefaultSubobject<UStaticMeshComponent>("Third");
	StaticMeshTrafficLightThird->SetupAttachment(StaticMeshTrafficLightBody);
	ThirdGreenLight = CreateDefaultSubobject<UStaticMeshComponent>("Third Green Light");
	ThirdGreenLight->SetupAttachment(StaticMeshTrafficLightThird);
	ThirdGreenLight->SetVisibility(false);
	ThirdYellowLight = CreateDefaultSubobject<UStaticMeshComponent>("Third Yellow Light");
	ThirdYellowLight->SetupAttachment(StaticMeshTrafficLightThird);
	ThirdYellowLight->SetVisibility(false);
	ThirdRedLight = CreateDefaultSubobject<UStaticMeshComponent>("Third Red Light");
	ThirdRedLight->SetupAttachment(StaticMeshTrafficLightThird);
}

void AAICarTrafficLightTwoLines::SwitchMainLights(ELightState NewLightState)
{
	Super::SwitchMainLights(NewLightState);

	switch (NewLightState)
	{
	case ELightState::Red:
	case ELightState::Pedestrians:
	case ELightState::Delay:
		ThirdRedLight->SetVisibility(true);
		ThirdGreenLight->SetVisibility(false);
		break;

	case ELightState::Green:
		ThirdRedLight->SetVisibility(false);
		ThirdGreenLight->SetVisibility(true);
		break;
	}
}
void AAICarTrafficLightTwoLines::SwitchYellowLight(bool Switch)
{
	Super::SwitchYellowLight(Switch);
	switch (Switch)
	{
	case  true:
		ThirdYellowLight->SetVisibility(true);
		break;

	case false:
		ThirdYellowLight->SetVisibility(false);
		break;
	}
}