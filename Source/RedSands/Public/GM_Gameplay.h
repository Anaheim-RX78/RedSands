// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitClass.h"
#include "GameFramework/GameModeBase.h"
#include "GM_Gameplay.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API AGM_Gameplay : public AGameModeBase
{
	GENERATED_BODY()
	

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void UpdateFogOfWar();

	UPROPERTY()
	UTextureRenderTarget2D* FogRenderTarget;
	
	UPROPERTY()
	TArray<AUnitClass*> PlayerUnits;

private:
	int32 PlayerTeamID;
};
