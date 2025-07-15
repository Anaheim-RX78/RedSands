// Fill out your copyright notice in the Description page of Project Settings.


#include "MCVUnit.h"
#include "CustomAIController.h"
#include "ScoutUnit.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"

void AMCVUnit::OnSelected_Implementation(bool bIsSelected)
{
	Super::OnSelected_Implementation(bIsSelected);
}

void AMCVUnit::OnDamaged_Implementation(float DamageAmount)
{
	  CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, Health);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s Health: %f, MaxHealth: %f"), *GetName(), CurrentHealth, Health));
    float HealthPercent = Health > 0.0f ? CurrentHealth / Health : 0.0f;
    GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,FString::Printf(TEXT("Percent: %f"), HealthPercent));
        // Update health bar
    if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
    {
        UUserWidget* HealthBar = Cast<UUserWidget>(HealthBarWidget->GetUserWidgetObject());
        if (HealthBar)
        {
            UProgressBar* ProgressBar = Cast<UProgressBar>(HealthBar->GetWidgetFromName(TEXT("PB_UnitHP")));
            if (ProgressBar)
            {
                ProgressBar->SetPercent(HealthPercent);
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Set ProgressBar to %f"), HealthPercent));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("HealthProgressBar not found in BP_UWUnitHP"));
            }
            HealthBarWidget->SetVisibility(CurrentHealth < Health);
            if (CurrentHealth < Health)
            {
                GetWorld()->GetTimerManager().SetTimer(
                    HealthBarHideTimer, 
                    [this]() { if (HealthBarWidget) HealthBarWidget->SetVisibility(false); }, 
                    5.0f, 
                    false
                );
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast HealthBarWidget to UUserWidget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthBarWidget or UserWidgetObject is null"));
    }
    if (CurrentHealth <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Destroyed"), *GetName()));
        CurrentTarget = nullptr;
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        Destroy();
    }
}

void AMCVUnit::BeginPlay()
{
	Super::BeginPlay();
	Health = 2000.f;
	Battery = 1000.f;
	Speed = 500.f;
	AttackRange = 0.f;
	AttackDamage = 0.f;
	AttackInterval = 0.f;
	bCanAttack = false;
	bCanMove = true;
	CurrentState = EUnitState::Idle;
	bFollowingOrders = false;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AMCVUnit::Ability()
{
	Super::Ability();
	
}

void AMCVUnit::StartProducingUnit(TSubclassOf<AActor> UnitClass)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	ARTSPlayerstate* PlayerStateT = PlayerController ? PlayerController->GetPlayerState<ARTSPlayerstate>(): nullptr;

	if (!PlayerStateT || !UnitClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerState or UnitClass"));
		return;
	}

	float CostMinerals = (UnitClass == UnitScoutClass ) ? 125.f:
	(UnitClass == UnitRailTankClass) ? 200.f : 170.f;
	
	if (PlayerStateT->IsMineralsAvailable(CostMinerals))
	{
		if (bIsProducing)
		{
			UE_LOG(LogTemp, Warning, TEXT("Already producing a unit, ignoring request"));
			return;
		}
		bIsProducing = true;
		PlayerStateT->DeductMinerals(CostMinerals);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BUILDING...")));
		
		GetWorld()->GetTimerManager().SetTimer(ProductionTimerHandle, [this, UnitClass]()
		{
			CompleteProduction(UnitClass);
			bIsProducing = false;
		}, 5.0f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough resources!"));
	}
}

void AMCVUnit::CompleteProduction(TSubclassOf<AActor> UnitClass)
{
	if (UnitClass)
	{
		FVector SpawnLocation = GetActorLocation() + SpawnOffset;
		FRotator SpawnRotation = GetActorRotation();
		GetWorld()->SpawnActor<AActor>(UnitClass, SpawnLocation, SpawnRotation);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BUILT!")));
	}
}
