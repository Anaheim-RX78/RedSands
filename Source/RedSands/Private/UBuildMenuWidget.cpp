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

	if (APlayerController* PC = GetOwningPlayer())
	{
		CachedPlayerState = PC->PlayerState;
		if (ARTSPlayerstate* MyPlayerState = Cast<ARTSPlayerstate>(CachedPlayerState))
		{
			MyPlayerState->OnMineralsChanged.AddUObject(this, &UUBuildMenuWidget::OnMineralsChanged);
			UE_LOG(LogTemp, Log, TEXT("NativeConstruct: Successfully bound to PlayerState, Minerals = %f"),
				MyPlayerState->Minerals);
			// Force initial update
			OnMineralsChanged(MyPlayerState->Minerals);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Failed to cast PlayerState, Type = %s"),
				*GetNameSafe(PC->PlayerState));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: GetOwningPlayer returned null"));
	}
}

void UUBuildMenuWidget::SetSelectedMCV(AMCVUnit* MCV)
{
	SelectedMCV = MCV;
}


void UUBuildMenuWidget::OnMineralsChanged(float NewMinerals)
{
	if (MineralsText)
	{
		MineralsText->SetText(FText::FromString(FString::Printf(TEXT("Minerals: %f"), NewMinerals)));
	}
}

FText UUBuildMenuWidget::GetMineralsText() const
{
	if (const ARTSPlayerstate* MyPlayerState = Cast<ARTSPlayerstate>(CachedPlayerState))
	{
		UE_LOG(LogTemp, Log, TEXT("GetMineralsText: Minerals = %f"), MyPlayerState->Minerals);
		return FText::FromString(FString::Printf(TEXT("Minerals: %f"), MyPlayerState->Minerals));
	}
	UE_LOG(LogTemp, Warning, TEXT("GetMineralsText: Failed to cast CachedPlayerState to AMyPlayerState"));
	return FText::FromString(TEXT("Minerals: 0"));
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
