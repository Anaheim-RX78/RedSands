// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMap.h"
#include "PlayerPawn.h"
#include "EngineUtils.h"
#include "ToDownHUD.h"
#include "UBuildMenuWidget.h"
#include "CameraPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API ACameraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	APlayerPawn* PlayerPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUBuildMenuWidget> BuildMenuWidgetClass;

	UPROPERTY()
	UUBuildMenuWidget* BuildMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY()
	UUserWidget* GameOverWidget;

	UPROPERTY()
	UUserWidget* VictoryWidget;
	
	UFUNCTION()
	void GameOver(bool winorlose);
protected:

	UPROPERTY()
	AActor* SelectedActor;

	UPROPERTY()
	TArray<AActor*> SelectedActors;

	UPROPERTY()
	TObjectPtr<AToDownHUD> TopDownHud;

	UPROPERTY()
	FVector2D SelectionStartPosition;

	UPROPERTY()
	FVector2D SelectionSize;

	UPROPERTY()
	AActor* PlayerMCV;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Input)
	UInputMap* InputMap;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void Rotate(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(const FInputActionValue& Value);
	
	UFUNCTION()
	void Select(const FInputActionValue& Value);

	UFUNCTION()
	void MultipleSelectStart(const FInputActionValue& Value);
	
	UFUNCTION()
	void MultipleSelectOnGoing(const FInputActionValue& Value);
	
	UFUNCTION()
	void MultipleSelectEnd(const FInputActionValue& Value);

	UFUNCTION()
	void SelectMCV(const FInputActionValue& Value);

	UFUNCTION()
	void SelectCombatUnits(const FInputActionValue& Value);
	
	UFUNCTION()
	void UnitAction(const FInputActionValue& Value);
	
	UFUNCTION()
	void AttackMove(const FInputActionValue& Value);
	
	UFUNCTION()
	void ShowBuildMenu(AMCVUnit* SelectedMCV);
	
	UFUNCTION()
	void UnitAbility(const FInputActionValue& Value);

	UFUNCTION()
	void AttackOrder(AActor* UnitActor, AActor* TargetActor);

	UFUNCTION()
	void MultiSelectActors(TArray<AActor*> BoxSelectActors);

};
