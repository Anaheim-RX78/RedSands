// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitClass.h"
#include "RTSPlayerState.h"
#include "MCVUnit.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API AMCVUnit : public AUnitClass
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Units")
	TSubclassOf<AActor> UnitScoutClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Units")
	TSubclassOf<AActor> UnitRailTankClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Units")
	TSubclassOf<AActor> UnitRocketTruckClass;

	UPROPERTY()
	bool bIsProducing = false;
	
	virtual void OnSelected_Implementation(bool bIsSelected) override;
	virtual void OnDamaged_Implementation(float DamageAmount) override;
	virtual void BeginPlay() override;
	virtual void Ability() override;

	UFUNCTION(BlueprintCallable)
	void StartProducingUnit(TSubclassOf<AActor> UnitClass);

	UFUNCTION()
	void CompleteProduction(TSubclassOf<AActor> UnitClass);

	UPROPERTY(EditAnywhere, Category = "Units")
	FVector SpawnOffset = FVector(0.f, -800.f, 0.f);

private:
	FTimerHandle ProductionTimerHandle;
	
};
