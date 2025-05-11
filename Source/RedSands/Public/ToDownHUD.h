// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ToDownHUD.generated.h"

/**
 * 
 */
UCLASS()
class REDSANDS_API AToDownHUD : public AHUD
{
	GENERATED_BODY()

private:
	bool bDrawSelectionBox = false;
	
	FVector2d SelectionStart;
	
	FVector2d SelectionBoxSize;

	FLinearColor SelectionColor= FLinearColor(1.f,0.f,0.f,.2f);

	protected:
	virtual void DrawHUD() override;

	public:
	virtual void ShowRectangleBox(FVector2D inSelectionBoxBegin, FVector2D inSelectionBoxSize);

	virtual void HideRectangleBox();
};
