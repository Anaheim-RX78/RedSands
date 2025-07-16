// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerstate.h"


ARTSPlayerstate::ARTSPlayerstate()
{
	Minerals=1000;
}

void ARTSPlayerstate::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Initial Minerals: %f"), Minerals));
	OnMineralsChanged.Broadcast(Minerals);
}



bool ARTSPlayerstate::IsMineralsAvailable(float UnitCost)
{
	if (UnitCost<= Minerals)
	{
		return true;
	}
	return false;
}

void ARTSPlayerstate::DeductMinerals(float UnitCost)
{
	Minerals=Minerals-UnitCost;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Minerals: %f"), Minerals));

	OnMineralsChanged.Broadcast(Minerals);
}

void ARTSPlayerstate::GainMinerals(float Amount)
{
	Minerals=Minerals+Amount;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Minerals: %f"), Minerals));

	OnMineralsChanged.Broadcast(Minerals);
}


