// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMap.h"
#include "PlayerPawn.h"
#include "CameraPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API ACameraPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	APlayerPawn* PlayerPawn;

	UPROPERTY()
	AActor* SelectedActor;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Input)
	UInputMap* InputMap;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void Rotate(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(const FInputActionValue& Value);
	
	UFUNCTION()
	void Select(const FInputActionValue& Value);
	
	UFUNCTION()
	void UnitAction(const FInputActionValue& Value);
};
