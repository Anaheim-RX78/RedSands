// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCVUnit.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UBuildMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API UUBuildMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	AMCVUnit* SelectedMCV;

	// Set the selected MCV when showing the widget
	void SetSelectedMCV(AMCVUnit* MCV);

	UPROPERTY(meta = (BindWidget))
	class UButton* UBScout;

	UPROPERTY(meta = (BindWidget))
	class UButton* UBRailTank;

	UPROPERTY(meta = (BindWidget))
	class UButton* UBRocketTruck;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UTextBlock* MineralsText;

	void OnMineralsChanged(float NewMinerals);

	UFUNCTION(BlueprintCallable, Category = "UI")
	FText GetMineralsText() const;
	
	UFUNCTION()
	void OnClickedScout();

	UFUNCTION()
	void OnClickedRailTank();

	UFUNCTION()
	void OnClickedRocketTruck();

protected:
	UPROPERTY()
	APlayerState* CachedPlayerState;
};
