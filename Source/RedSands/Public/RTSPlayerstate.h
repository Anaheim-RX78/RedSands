// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RTSPlayerstate.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMineralsChanged, float);
/**
 * 
 */
UCLASS()
class REDSANDS_API ARTSPlayerstate : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Minerals = 1000;

	UFUNCTION()
	void BeginPlay() override;
	
	UFUNCTION()
	bool IsMineralsAvailable(float UnitCost);

	UFUNCTION()
	void DeductMinerals(float UnitCost);

	UFUNCTION()
	void GainMinerals(float Amount);

	FOnMineralsChanged OnMineralsChanged;
};
