// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoutUnit.h"

#include "CustomAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


void AScoutUnit::OnSelected_Implementation(bool bIsSelected)
{
	Super::OnSelected_Implementation(bIsSelected);
}

void AScoutUnit::OnDamaged_Implementation(float DamageAmount)
{
	Super::OnDamaged_Implementation(DamageAmount);
    Health -= DamageAmount;
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Unit %s: Took %f damage, Current Health: %f"), *GetName(), DamageAmount, Health));
    if (Health <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Destroyed"), *GetName()));
        CurrentTarget = nullptr;
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        Destroy();
    }
}

void AScoutUnit::BeginPlay()
{
	Super::BeginPlay();
	Health = 100.f;
	Battery = 1000.f;
	Speed = 1000.f;
	AttackRange = 500.f;
	AttackDamage = 25.f;
	AttackInterval = 10.f;
	bCanAttack = true;
	bCanMove = true;
	CurrentState = EUnitState::Idle;
	bFollowingOrders = false;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AScoutUnit::AttackAction(AActor* Enemy)
{
    // Check if the enemy is valid and the unit can attack
    if (!Enemy || !bCanAttack || !IsValid(Enemy))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack failed - Enemy invalid or cannot attack"), *GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle); // Clear any existing timer
        return;
    }

    // Check if the enemy implements the damage interface
    if (Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
    {
        float DistanceToEnemy = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());

        if (DistanceToEnemy <= AttackRange)
        {
            // Enemy is in range, attack
            CurrentState = EUnitState::Attacking;
            bFollowingOrders = true;

            // Apply damage by calling the interface function directly on the enemy
            IDamageInterface* DamageableEnemy = Cast<IDamageInterface>(Enemy);
            if (DamageableEnemy)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Unit %s: Attacking %s, dealing %f damage, AttackInterval: %f"), *GetName(), *Enemy->GetName(), AttackDamage, AttackInterval));
                DamageableEnemy->Execute_OnDamaged(Enemy, AttackDamage);
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack failed - Enemy cast to IDamageInterface failed"), *GetName()));
                CurrentState = EUnitState::Idle;
                bFollowingOrders = false;
                CurrentTarget = nullptr;
                GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
                return;
            }

            // Clear any existing timer to prevent overlap
            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

            // Start the attack interval timer
            GetWorld()->GetTimerManager().SetTimer(
                AttackTimerHandle,
                [this, Enemy]()
                {
                    // Check if the enemy is still valid
                    if (Enemy && IsValid(Enemy))
                    {
                        float CurrentDistance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
                        if (CurrentDistance <= AttackRange)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Unit %s: Enemy %s still in range, continuing attack"), *GetName(), *Enemy->GetName()));
                            AttackAction(Enemy);
                        }
                        else
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Unit %s: Enemy %s out of range (%f > %f), pursuing"), *GetName(), *Enemy->GetName(), CurrentDistance, AttackRange));
                            PursueEnemy(Enemy);
                            // Set a timer to check again after a short delay
                            GetWorld()->GetTimerManager().SetTimer(
                                AttackTimerHandle,
                                [this, Enemy]()
                                {
                                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Unit %s: Re-checking attack/pursuit for %s"), *GetName(), *Enemy->GetName()));
                                    AttackAction(Enemy);
                                },
                                0.5f, // Short delay for pursuit re-check
                                false
                            );
                        }
                    }
                    else
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack stopped - Enemy invalid in timer"), *GetName()));
                        CurrentState = EUnitState::Idle;
                        bFollowingOrders = false;
                        CurrentTarget = nullptr;
                        if (ACustomAIController* AIController = Cast<ACustomAIController>(GetController()))
                        {
                            AIController->StopMovement();
                        }
                    }
                },
                AttackInterval > 0.0f ? AttackInterval : 1.0f, // Ensure a valid interval
                false // One-shot timer
            );
        }
        else
        {
            // Enemy is out of range, pursue
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Unit %s: Enemy %s out of range (%f > %f), starting pursuit"), *GetName(), *Enemy->GetName(), DistanceToEnemy, AttackRange));
            PursueEnemy(Enemy);
            // Clear any existing timer to prevent overlap
            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
            // Set a timer to re-check after a short delay
            GetWorld()->GetTimerManager().SetTimer(
                AttackTimerHandle,
                [this, Enemy]()
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Unit %s: Re-checking attack/pursuit for %s"), *GetName(), *Enemy->GetName()));
                    AttackAction(Enemy);
                },
                0.5f, // Short delay for pursuit re-check
                false
            );
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack failed - Enemy does not implement IDamageInterface"), *GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
    }
}

