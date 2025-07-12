// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoutUnit.h"

#include "CustomAIController.h"
#include "EngineUtils.h"

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
    AggroRange = 500.f;
	AttackDamage = 25.f;
	AttackInterval = 2.f;
	bCanAttack = true;
	bCanMove = true;
	CurrentState = EUnitState::Idle;
	bFollowingOrders = false;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AScoutUnit::AttackAction(AActor* Enemy)
{
    // Validate the enemy
    if (!Enemy || !IsValid(Enemy) || !bCanAttack)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack failed - Enemy invalid or cannot attack"), *GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
        return;
    }

    // Check if the enemy implements the damage interface
    if (!Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack failed - Enemy does not implement IDamageInterface"), *GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
        return;
    }

    // Account for collision capsule radii
    float CapsuleOffset = 200.0f; // Adjust based on actual capsule sizes
    float EffectiveAttackRange = AttackRange + 10.0f + CapsuleOffset; // 710.0f
    float DistanceToEnemy = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Unit %s: Distance to %s: %f (Effective Range: %f)"), *GetName(), *Enemy->GetName(), DistanceToEnemy, EffectiveAttackRange));

    if (DistanceToEnemy <= EffectiveAttackRange)
    {
        // Enemy is in range, attack
        if (GetWorld()->GetTimerManager().IsTimerActive(AttackTimerHandle))
        {
            return; // Still in cooldown
        }

        CurrentState = EUnitState::Attacking;
        bFollowingOrders = true;
        CurrentTarget = Enemy;

        // Apply damage
        if (IDamageInterface* DamageableEnemy = Cast<IDamageInterface>(Enemy))
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

        // Start attack cooldown timer
        GetWorld()->GetTimerManager().SetTimer(
            AttackTimerHandle,
            [this, Enemy]()
            {
                if (CurrentState == EUnitState::Attacking && Enemy && IsValid(Enemy))
                {
                    AttackAction(Enemy); // Continue attacking if still in range
                }
            },
            AttackInterval > 0.0f ? AttackInterval : 1.0f,
            false
        );
    }
    else
    {
        // Enemy out of range, pursue
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Unit %s: Enemy %s out of range (%f > %f), pursuing"), *GetName(), *Enemy->GetName(), DistanceToEnemy, EffectiveAttackRange));
        CurrentState = EUnitState::Pursuing;
        bFollowingOrders = true;
        CurrentTarget = Enemy;
        PursueEnemy(Enemy);
        GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
    }
}

void AScoutUnit::ProximityAggro()
{
    Super::ProximityAggro();
    if (CurrentTarget && IsValid(CurrentTarget)) return;

    AActor* ClosestEnemy = nullptr;
    float ClosestDistance = MAX_FLT;
    for (TActorIterator<AUnitClass> It(GetWorld()); It; ++It)
    {
        AUnitClass* OtherUnit = *It;
        if (OtherUnit && OtherUnit != this && OtherUnit->TeamIDU != TeamIDU)
        {
            float Distance = FVector::Dist(GetActorLocation(), OtherUnit->GetActorLocation());
            if (Distance < ClosestDistance && Distance <= AggroRange)
            {
                ClosestDistance = Distance;
                ClosestEnemy = OtherUnit;
            }
        }
    }

    if (ClosestEnemy)
    {
        CurrentTarget = ClosestEnemy;
        bFollowingOrders = true;
        float CapsuleOffset = 200.0f; // Match AttackAction
        if (ClosestDistance <= AttackRange + 10.0f + CapsuleOffset)
        {
            CurrentState = EUnitState::Attacking;
        }
        else
        {
            CurrentState = EUnitState::Pursuing;
            PursueEnemy(ClosestEnemy);
        }
    }
}

void AScoutUnit::Ability()
{
    Super::Ability();
}

