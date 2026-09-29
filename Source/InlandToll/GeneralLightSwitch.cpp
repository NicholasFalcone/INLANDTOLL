
#include "GeneralLightSwitch.h"

void AGeneralLightSwitch::OnInteract()
{
    ToggleLights();
}

void AGeneralLightSwitch::ToggleLights()
{
    bIsLightOn = !bIsLightOn;
    for (ALight* Light : Lights)
    {
        if (Light)
        {
            Light->SetEnabled(bIsLightOn);
        }
    }
    OnLightsToggled.Broadcast();
}

void AGeneralLightSwitch::SetLight(bool bNewState)
{
    bIsLightOn = bNewState;
    for (ALight* Light : Lights)
    {
        if (Light)
        {
            Light->SetEnabled(bNewState);
        }
    }
    OnLightsToggled.Broadcast();
}

void AGeneralLightSwitch::BeginPlay()
{
    Super::BeginPlay();
    // Any initialization code for the light switch can go here
    SetLight(bIsLightOn);
}
