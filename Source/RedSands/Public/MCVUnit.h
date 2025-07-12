// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitClass.h"
#include "MCVUnit.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API AMCVUnit : public AUnitClass
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bBuildMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float EnergizedAlloy;
	
	virtual void OnSelected_Implementation(bool bIsSelected) override;
	virtual void OnDamaged_Implementation(float DamageAmount) override;
	virtual void BeginPlay() override;
	virtual void Ability() override;
};
