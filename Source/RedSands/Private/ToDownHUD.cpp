// Fill out your copyright notice in the Description page of Project Settings.


#include "ToDownHUD.h"

void AToDownHUD::DrawHUD()
{
	Super::DrawHUD();
	if (bDrawSelectionBox)
	{
		DrawRect(SelectionColor,SelectionStart.X,SelectionStart.Y,SelectionBoxSize.X,SelectionBoxSize.Y);
	}
}

void AToDownHUD::ShowRectangleBox(FVector2D inSelectionBoxBegin, FVector2D inSelectionBoxSize)
{
	SelectionStart = inSelectionBoxBegin;
	SelectionBoxSize = inSelectionBoxSize;
	bDrawSelectionBox = true;
}

void AToDownHUD::HideRectangleBox()
{
	bDrawSelectionBox = false;
}
