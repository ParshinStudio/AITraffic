// Fill out your copyright notice in the Description page of Project Settings.


#include "AICarTrafficLightWithArm.h"

AAICarTrafficLightWithArm::AAICarTrafficLightWithArm()
{
	StaticMeshTrafficLightArm = CreateDefaultSubobject<UStaticMeshComponent>("MainArm");
	StaticMeshTrafficLightArm->SetupAttachment(GetRootComponent());

	StaticMeshTrafficLightSecondary = CreateDefaultSubobject<UStaticMeshComponent>("Secondary");
	StaticMeshTrafficLightSecondary->SetupAttachment(StaticMeshTrafficLightBody);
	SecondaryGreenLight = CreateDefaultSubobject<UStaticMeshComponent>("Secondary Green Light");
	SecondaryGreenLight->SetupAttachment(StaticMeshTrafficLightSecondary);
	SecondaryGreenLight->SetVisibility(false);
	SecondaryYellowLight = CreateDefaultSubobject<UStaticMeshComponent>("Secondary Yellow Light");
	SecondaryYellowLight->SetupAttachment(StaticMeshTrafficLightSecondary);
	SecondaryYellowLight->SetVisibility(false);
	SecondaryRedLight = CreateDefaultSubobject<UStaticMeshComponent>("Secondary Red Light");
	SecondaryRedLight->SetupAttachment(StaticMeshTrafficLightSecondary);
}

void AAICarTrafficLightWithArm::SwitchMainLights(ELightState NewLightState)
{
	Super::SwitchMainLights(NewLightState);

	switch (NewLightState)
	{
	case ELightState::Red:
	case ELightState::Pedestrians:
	case ELightState::Delay:
		SecondaryRedLight->SetVisibility(true);
		SecondaryGreenLight->SetVisibility(false);
		break;

	case ELightState::Green:
		SecondaryRedLight->SetVisibility(false);
		SecondaryGreenLight->SetVisibility(true);
		break;
	}
}
void AAICarTrafficLightWithArm::SwitchYellowLight(bool Switch)
{
	Super::SwitchYellowLight(Switch);
	switch (Switch)
	{
	case  true:
		SecondaryYellowLight->SetVisibility(true);
		break;

	case false:
		SecondaryYellowLight->SetVisibility(false);
		break;
	}
}