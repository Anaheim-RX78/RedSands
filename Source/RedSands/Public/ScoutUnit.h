// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitClass.h"
#include "ScoutUnit.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API AScoutUnit : public AUnitClass
{
	GENERATED_BODY()
public:
	virtual void OnSelected_Implementation(bool bIsSelected) override;
};
