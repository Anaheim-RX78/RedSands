// Fill out your copyright notice in the Description page of Project Settings.


#include "UBuildMenuWidget.h"

#include "Components/Button.h"

void UUBuildMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UBScout)
	{
		UBScout->OnClicked.AddDynamic(this, &UUBuildMenuWidget::OnClickedScout);
	}
	if (UBRailTank)
	{
		UBRailTank->OnClicked.AddDynamic(this, &UUBuildMenuWidget::OnClickedRailTank);
	}
	if (UBRocketTruck)
	{
		UBRocketTruck->OnClicked.AddDynamic(this, &UUBuildMenuWidget::OnClickedRocketTruck);
	}
}

void UUBuildMenuWidget::SetSelectedMCV(AMCVUnit* MCV)
{
	SelectedMCV = MCV;
}



void UUBuildMenuWidget::OnClickedScout()
{
	if (SelectedMCV)
	{
		SelectedMCV->StartProducingUnit(SelectedMCV->UnitScoutClass);
	}
}

void UUBuildMenuWidget::OnClickedRailTank()
{
	if (SelectedMCV)
	{
		SelectedMCV->StartProducingUnit(SelectedMCV->UnitRailTankClass);
	}
}

void UUBuildMenuWidget::OnClickedRocketTruck()
{
	if (SelectedMCV)
	{
		SelectedMCV->StartProducingUnit(SelectedMCV->UnitRocketTruckClass);
	}
}
