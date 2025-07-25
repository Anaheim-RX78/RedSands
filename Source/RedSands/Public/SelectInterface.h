// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectInterface.generated.h"

UINTERFACE(MinimalAPI)
class USelectInterface : public UInterface
{
	GENERATED_BODY()
};

class REDSANDS_API ISelectInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	void OnSelected(bool bIsSelected);
};

