// Fill out your copyright notice in the Description page of Project Settings.


#include "RailTankUnit.h"
#include "CustomAIController.h"
#include "EngineUtils.h"
#include "Components/ProgressBar.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void ARailTankUnit::OnSelected_Implementation(bool bIsSelected)
{
	Super::OnSelected_Implementation(bIsSelected);
}

void ARailTankUnit::OnDamaged_Implementation(float DamageAmount)
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

void ARailTankUnit::BeginPlay()
{
	Super::BeginPlay();
    Health = 250.f;
    CurrentHealth = 250.f;
    Battery = 1000.f;
    Speed = 700.f;
    AttackRange = 1500.f;
    AggroRange = 1500.f;
    AcceptanceRadius = 1500.f;
    AttackDamage = 60.f;
    AttackInterval = 3.5f;
    bCanAttack = true;
    bCanMove = true;
    CurrentState = EUnitState::Idle;
    bFollowingOrders = false;
    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ARailTankUnit::AttackAction(AActor* Enemy)
{
if (!IsValid(this) || !GetWorld() || !IsValid(GetController()))
    {
        UE_LOG(LogTemp, Warning, TEXT("ScoutUnit %s: AttackAction aborted - Invalid unit, world, or controller"), *GetName());
        return;
    }

    // Validate the enemy
    if (!Enemy || !IsValid(Enemy) || !bCanAttack)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ScoutUnit %s: Attack failed - Enemy invalid or cannot attack"), *GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
        }
        return;
    }

    // Check if the enemy implements the damage interface
    if (!Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ScoutUnit %s: Attack failed - %s does not implement IDamageInterface"), *GetName(), *Enemy->GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
        }
        return;
    }

    // Check team
    int32 TargetTeamID = 1;
    if (AUnitClass* Unit = Cast<AUnitClass>(Enemy))
    {
        TargetTeamID = Unit->TeamIDU;
    }
    if (TargetTeamID == TeamIDU)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ScoutUnit %s: Attack failed - %s is same team"), *GetName(), *Enemy->GetName()));
        CurrentState = EUnitState::Idle;
        bFollowingOrders = false;
        CurrentTarget = nullptr;
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
        }
        return;
    }

    // Check cooldown
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime < AttackInterval)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("ScoutUnit %s: Attack on cooldown, %f seconds remaining"), *GetName(), AttackInterval - (CurrentTime - LastAttackTime)));
        return;
    }

    // Account for collision capsule radii
    float CapsuleOffset = 200.0f;
    float EffectiveAttackRange = AttackRange + 10.0f + CapsuleOffset; // 710.0f + 200.0f
    float DistanceToEnemy = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("ScoutUnit %s: Distance to %s: %f (Effective Range: %f)"), *GetName(), *Enemy->GetName(), DistanceToEnemy, EffectiveAttackRange));

    if (DistanceToEnemy <= EffectiveAttackRange)
    {
        // Stop movement to prevent overshooting
        if (AController* StopController = GetController())
        {
            StopController->StopMovement();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ScoutUnit %s: No valid controller to stop movement"), *GetName());
        }

        // Enemy is in range, attack
        CurrentState = EUnitState::Attacking;
        bFollowingOrders = true;
        CurrentTarget = Enemy;

        // Apply damage
        if (IDamageInterface* DamageableEnemy = Cast<IDamageInterface>(Enemy))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("ScoutUnit %s: Attacking %s, dealing %f damage"), *GetName(), *Enemy->GetName(), AttackDamage));
            DamageableEnemy->Execute_OnDamaged(Enemy, AttackDamage);

            // Visual and sound effects
            if (MuzzleFlashComponent && MuzzleFlashSystem && IsValid(MuzzleFlashComponent))
            {
                MuzzleFlashComponent->ActivateSystem(true);
                FVector EffectLocation = GetMesh() && GetMesh()->DoesSocketExist("S_Muzzle")
                    ? GetMesh()->GetSocketLocation("S_Muzzle")
                    : MuzzleFlashComponent->GetComponentLocation();
                if (FireSound && IsValid(FireSound))
                {
                    UGameplayStatics::PlaySoundAtLocation(this, FireSound, EffectLocation);
                }
                if (GetWorld())
                {
                    DrawDebugPoint(GetWorld(), EffectLocation, 10.0f, FColor::Red, false, 5.0f);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("ScoutUnit %s: MuzzleFlashComponent or MuzzleFlashSystem invalid"), *GetName());
            }

            // Update last attack time
            LastAttackTime = CurrentTime;
            UE_LOG(LogTemp, Log, TEXT("ScoutUnit %s: Attacked %s, LastAttackTime = %f"), *GetName(), *Enemy->GetName(), LastAttackTime);
        }
        else
        {
          //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ScoutUnit %s: Attack failed - Enemy cast to IDamageInterface failed"), *GetName()));
            CurrentState = EUnitState::Idle;
            bFollowingOrders = false;
            CurrentTarget = nullptr;
            if (GetWorld())
            {
                GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
            }
            return;
        }

        // Start attack cooldown timer
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().SetTimer(
                AttackTimerHandle,
                [this]()
                {
                    if (!IsValid(this) || !GetWorld())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("ScoutUnit %s: Attack timer callback aborted - Invalid unit or world"), *GetName());
                        return;
                    }
                    if (CurrentState == EUnitState::Attacking && CurrentTarget && IsValid(CurrentTarget))
                    {
                        AttackAction(CurrentTarget); // Continue attacking
                    }
                    else
                    {
                        CurrentState = EUnitState::Idle;
                        bFollowingOrders = false;
                        CurrentTarget = nullptr;
                        if (GetWorld())
                        {
                            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
                        }
                        UE_LOG(LogTemp, Log, TEXT("ScoutUnit %s: Attack timer stopped - Target invalid or not attacking"), *GetName());
                    }
                },
                AttackInterval > 0.0f ? AttackInterval : 1.0f,
                false
            );
        }
    }
    else
    {
        // Enemy out of range, pursue
       // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("ScoutUnit %s: Enemy %s out of range (%f > %f), pursuing"), *GetName(), *Enemy->GetName(), DistanceToEnemy, EffectiveAttackRange));
        CurrentState = EUnitState::Pursuing;
        bFollowingOrders = true;
        CurrentTarget = Enemy;
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
        }
        PursueEnemy(Enemy);
    }
}

void ARailTankUnit::ProximityAggro()
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
void ARailTankUnit::Ability()
{
	Super::Ability();
}
