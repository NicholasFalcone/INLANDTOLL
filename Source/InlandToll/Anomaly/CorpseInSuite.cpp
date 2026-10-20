// Fill out your copyright notice in the Description page of Project Settings.

#include "Anomaly/CorpseInSuite.h"


void ACorpseInSuite::SetSuitOpen(bool bOpen)
{
	isOpen = bOpen;
	if (isOpen)
	{
		CurrentTimeOpen = 0.0f;
	}
}

void ACorpseInSuite::Tick(float DeltaTime)
{
	if (isOpen)
	{
		CurrentTimeOpen += DeltaTime;
		if (CurrentTimeOpen >= MaxTimeOpen)
		{
    	    isOpen = false;
            CurrentTimeOpen = 0.0f;
            OnMaxTimeReached();
		}
	}
}

void ACorpseInSuite::OnMaxTimeReached()
{
    OnMaxTimeReachedEvent.Broadcast();
}
