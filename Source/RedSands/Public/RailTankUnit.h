// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitClass.h"
#include "RailTankUnit.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API ARailTankUnit : public AUnitClass
{
	GENERATED_BODY()
	virtual void OnSelected_Implementation(bool bIsSelected) override;
	virtual void OnDamaged_Implementation(float DamageAmount) override;
	virtual void BeginPlay() override;
	virtual void AttackAction(AActor* Enemy) override;
	virtual void ProximityAggro() override;
	virtual void Ability() override;
};
