// Fill out your copyright notice in the Description page of Project Settings.

#include "AICarTrafficLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"

#include "AICarTriggerVolume.h"
#include "AICarPedestrianLight.h"

AAICarTrafficLight::AAICarTrafficLight()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = _RootComponent;
	StaticMeshTrafficLightBody = CreateDefaultSubobject<UStaticMeshComponent>("MainBody");
	StaticMeshTrafficLightBody->SetupAttachment(RootComponent);

	StaticMeshTrafficLightPrimary = CreateDefaultSubobject<UStaticMeshComponent>("Primary");
	StaticMeshTrafficLightPrimary->SetupAttachment(StaticMeshTrafficLightBody);
	PrimaryGreenLight = CreateDefaultSubobject<UStaticMeshComponent>("Primary Green Light");
	PrimaryGreenLight->SetupAttachment(StaticMeshTrafficLightPrimary);
	PrimaryGreenLight->SetVisibility(false);
	PrimaryYellowLight = CreateDefaultSubobject<UStaticMeshComponent>("Primary Yellow Light");
	PrimaryYellowLight->SetupAttachment(StaticMeshTrafficLightPrimary);
	PrimaryYellowLight->SetVisibility(false);
	PrimaryRedLight = CreateDefaultSubobject<UStaticMeshComponent>("Primary Red Light");
	PrimaryRedLight->SetupAttachment(StaticMeshTrafficLightPrimary);
}

void AAICarTrafficLight::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < LightTriggerBoxes.Num(); i++)
	{
		LightTriggerBoxes[i]->SetGreenLightTimer(GreenLightTime);
		LightTriggerBoxes[i]->SetIsMainDirection(IsMainDirection);
	}

	if (!IsParentLight) return;
	ChangeLightState();
	for (int i = 0; i < ChildrenLights.Num(); i++)
	{
		ChildrenLights[i]->YellowActiveTimer = YellowActiveTimer;
	}
}

void AAICarTrafficLight::ChangeLightState()
{
	switch (LightState)
	{
	case ELightState::Green:
		if (DelayAfterGreen != 0.0f)
		{
			SetDelayState();
			GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, DelayAfterGreen - YellowActiveTimer);
			PreviousLightState = ELightState::Green;
			break;
		}
		else
		{
			SetRedState();
			GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, RedLightTime - YellowActiveTimer);
			break;
		}

	case ELightState::Red:
		if (PedestriansTime == 0.0f)
		{
			if (DelayAfterRed != 0.0f)
			{
				SetDelayState();
				GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, DelayAfterRed - YellowActiveTimer);
				PreviousLightState = ELightState::Red;
				break;
			}
			else
			{
				GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, GreenLightTime - YellowActiveTimer);
				SetGreenState();
				break;
			}
		}
		else
		{
			if (DelayAfterRed != 0.0f)
			{
				SetDelayState();
				GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, DelayAfterRed - YellowActiveTimer);
				PreviousLightState = ELightState::Red;
				break;
			}
			else
			{
				GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, PedestriansTime - YellowActiveTimer);
				SetPedestriansState();
				break;
			}
		}

	case ELightState::Pedestrians:
		if (DelayAfterPedestrians != 0.0f)
		{
			SetDelayState();
			GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, DelayAfterPedestrians - YellowActiveTimer);
			PreviousLightState = ELightState::Pedestrians;
			break;
		}
		else
		{
			GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, GreenLightTime - YellowActiveTimer);
			SetGreenState();
			break;
		}

	case ELightState::Delay:
		switch (PreviousLightState)
		{
		case ELightState::Green:
			SetRedState();
			GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, RedLightTime - YellowActiveTimer);
			break;

		case ELightState::Red:
			if (PedestriansTime == 0.0f)
			{
				SetGreenState();
				GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, GreenLightTime - YellowActiveTimer);
				break;
			}
			else
			{
				GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, PedestriansTime - YellowActiveTimer);
				SetPedestriansState();
				break;
			}
		case ELightState::Pedestrians:
			GetWorldTimerManager().SetTimer(MainLightSwitchHandle, this, &AAICarTrafficLight::ChangeLightState, 1.0f, false, GreenLightTime - YellowActiveTimer);
			SetGreenState();
			break;
		}
	}
}

void AAICarTrafficLight::SetGreenState()
{
	SetLightCondition(ELightState::Green);
	for (int i = 0; i < ChildrenLights.Num(); i++)
	{
		if (ChildrenLights[i]->IsInSameDirection == true)
		{
			ChildrenLights[i]->SetLightCondition(ELightState::Green);
		}
		else
		{
			ChildrenLights[i]->SetLightCondition(ELightState::Red);
		}
	}
}
void AAICarTrafficLight::SetRedState()
{
	SetLightCondition(ELightState::Red);
	for (int i = 0; i < ChildrenLights.Num(); i++)
	{
		if (ChildrenLights[i]->IsInSameDirection == true)
		{
			ChildrenLights[i]->SetLightCondition(ELightState::Red);
		}
		else
		{
			ChildrenLights[i]->SetLightCondition(ELightState::Green);
		}
	}
}
void AAICarTrafficLight::SetPedestriansState()
{
	SetLightCondition(ELightState::Pedestrians);
	for (int i = 0; i < ChildrenLights.Num(); i++)
	{
		ChildrenLights[i]->SetLightCondition(ELightState::Pedestrians);
	}
}
void AAICarTrafficLight::SetDelayState()
{
	SetLightCondition(ELightState::Delay);
	for (int i = 0; i < ChildrenLights.Num(); i++)
	{
		ChildrenLights[i]->SetLightCondition(ELightState::Delay);
	}
}


void AAICarTrafficLight::SetLightCondition(ELightState NewLightState)
{
	if (LightState == ELightState::Green || NewLightState == ELightState::Green)
	{
		YellowLightState = true;
		SwitchYellowLight(YellowLightState);
	}

	LightState = NewLightState;
	GetWorldTimerManager().SetTimer(YellowLightDelayHandle, this, &AAICarTrafficLight::SetLightConditionResume, 1.0, false, YellowActiveTimer);
}
void AAICarTrafficLight::SetLightConditionResume()
{
	SwitchMainLights(LightState);
	if (YellowLightState)
	{
		YellowLightState = false;
		SwitchYellowLight(YellowLightState);
	}

	if (LightTriggerBoxes.Num() != 0)
	{
		if (LightState == ELightState::Green)
		{
			for (int i = 0; i < LightTriggerBoxes.Num(); i++)
			{
				LightTriggerBoxes[i]->SetLetPassCrossRoads(true);
			}
		}
		else
		{
			for (int i = 0; i < LightTriggerBoxes.Num(); i++)
			{
				LightTriggerBoxes[i]->SetLetPassCrossRoads(false);
			}
		}
	}
	if (PedestrianLights.Num() != 0)
	{
		if (LightState == ELightState::Pedestrians)
		{
			for (int i = 0; i < PedestrianLights.Num(); i++)
			{
				PedestrianLights[i]->SetPedestrianLightState(true);
			}
		}
		else
		{
			for (int i = 0; i < PedestrianLights.Num(); i++)
			{
				PedestrianLights[i]->SetPedestrianLightState(false);
			}
		}
	}
}

void AAICarTrafficLight::SwitchMainLights(ELightState NewLightState) 
{
	switch (NewLightState)
	{
	case ELightState::Red:
	case ELightState::Pedestrians:
	case ELightState::Delay:
		PrimaryRedLight->SetVisibility(true);
		PrimaryGreenLight->SetVisibility(false);
		break;

	case ELightState::Green:
		PrimaryRedLight->SetVisibility(false);
		PrimaryGreenLight->SetVisibility(true);
		break;
	}
}
void AAICarTrafficLight::SwitchYellowLight(bool Switch)
{
	switch (Switch)
	{
	case  true:
		PrimaryYellowLight->SetVisibility(true);
		break;

	case false:
		PrimaryYellowLight->SetVisibility(false);
		break;
	}
}