// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REDSANDS_API ISelectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Selection")
	void OnSelected(bool bIsSelected);
};
