// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CustomAIController.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API ACustomAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACustomAIController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
};
