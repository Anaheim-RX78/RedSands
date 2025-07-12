#include "UnitClass.h"
#include "AIController.h"
#include "CustomAIController.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AUnitClass::AUnitClass()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetVisibility(false);
	
	PlaceholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	PlaceholderMesh->SetupAttachment(RootComponent);
	PlaceholderMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlaceholderMesh->SetCollisionProfileName(TEXT("BlockAll"));
	
	SelectionBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionBase"));
	SelectionBase->SetupAttachment(RootComponent);
	SelectionBase->SetVisibility(false);
	SelectionBase->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SelectionBase->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	SelectionBase->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetCharacterMovement()->MaxWalkSpeed = 0;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACustomAIController::StaticClass();
}


void AUnitClass::BeginPlay()
{
	Super::BeginPlay();
}

void AUnitClass::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EUnitState::Moving)
    {
        if (bAttackMove)
        {
            UpdateAttackMove(); // Check for enemies during movement
        }

        if (GetCharacterMovement()->Velocity.IsNearlyZero() && !bFollowingOrders)
        {
            CurrentState = EUnitState::Idle;
            if (CurrentTarget && IsValid(CurrentTarget))
            {
                float CapsuleOffset = 200.0f;
                float EffectiveAttackRange = AttackRange + 10.0f + CapsuleOffset; // 710.0f
                float DistanceToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
                UE_LOG(LogTemp, Log, TEXT("Unit %s: Move completed, Distance to %s: %f (Effective Range: %f)"), 
                       *GetName(), *CurrentTarget->GetName(), DistanceToTarget, EffectiveAttackRange);
                if (DistanceToTarget <= EffectiveAttackRange)
                {
                    CurrentState = EUnitState::Attacking;
                    bFollowingOrders = true;
                }
                else
                {
                    CurrentState = EUnitState::Pursuing;
                    bFollowingOrders = true;
                    PursueEnemy(CurrentTarget);
                }
            }
            else if (bAttackMove)
            {
                // Reached destination, no enemies found
                CurrentState = EUnitState::Idle;
                bAttackMove = false;
                bFollowingOrders = false;
                AttackMoveTarget = FVector::ZeroVector;
                UE_LOG(LogTemp, Log, TEXT("Unit %s: Reached attack move target %s, no enemies found"), *GetName(), *AttackMoveTarget.ToString());
            }
        }
    }
    else if (CurrentState == EUnitState::Idle && !bFollowingOrders)
    {
        ProximityAggro();
    }
    else if (CurrentState == EUnitState::Attacking)
    {
        AttackAction(CurrentTarget);
    }
    else if (CurrentState == EUnitState::Pursuing)
    {
        if (CurrentTarget && IsValid(CurrentTarget))
        {
            float CapsuleOffset = 200.0f;
            float EffectiveAttackRange = AttackRange + 10.0f + CapsuleOffset; // 710.0f
            float DistanceToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Unit %s: Pursuing %s, Distance: %f (Effective Range: %f)"), *GetName(), *CurrentTarget->GetName(), DistanceToTarget, EffectiveAttackRange));
            if (DistanceToTarget <= EffectiveAttackRange)
            {
                CurrentState = EUnitState::Attacking;
                bFollowingOrders = true;
                AttackAction(CurrentTarget);
            }
            else
            {
                PursueEnemy(CurrentTarget);
            }
        }
        else
        {
            CurrentState = EUnitState::Idle;
            bFollowingOrders = false;
            CurrentTarget = nullptr;
            if (ACustomAIController* AIController = Cast<ACustomAIController>(GetController()))
            {
                AIController->StopMovement();
            }
        }
    }
}
void AUnitClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AUnitClass::OnSelected_Implementation(bool bIsSelected)
{
	ISelectInterface::OnSelected_Implementation(bIsSelected);

	SelectionBase->SetVisibility(bIsSelected);
}

void AUnitClass::OnDamaged_Implementation(float DamageAmount)
{
	IDamageInterface::OnDamaged_Implementation(DamageAmount);
}


void AUnitClass::MovementAction(FVector Location)
{
	if (bCanMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Trying to move to: %s"), *Location.ToString());
		CurrentState = EUnitState::Moving;
		bFollowingOrders = true;

		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIController found: %s"), *AIController->GetName());
			AIController->MoveToLocation(Location, 20.0f);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No AIController found for: %s"), *GetName());
		}
	}
}

void AUnitClass::ProximityAggro()
{
	
}
void AUnitClass::AttackAction(AActor* Enemy)
{

}

void AUnitClass::Ability()
{
	
}

void AUnitClass::PursueEnemy(AActor* Enemy)
{
	if (Enemy && IsValid(Enemy))
	{
		if (ACustomAIController* AIController = Cast<ACustomAIController>(GetController()))
		{
			CurrentState = EUnitState::Pursuing;
			bFollowingOrders = true;
			float AcceptanceRadius = 350.0f; // Reduced to account for capsule radii (~90–115 units each)
			EPathFollowingRequestResult::Type Result = AIController->MoveToActor(Enemy, AcceptanceRadius);
			/*UE_LOG(LogTemp, Log, TEXT("Unit %s: Pursuing %s with acceptance radius %f, MoveToActor result: %d"), 
				   *GetName(), *Enemy->GetName(), AcceptanceRadius, (int32)Result);*/

			// Debug visualization
			DrawDebugSphere(GetWorld(), Enemy->GetActorLocation(), AcceptanceRadius, 12, FColor::Blue, false, 0.1f);
			DrawDebugSphere(GetWorld(), Enemy->GetActorLocation(), AttackRange, 12, FColor::Red, false, 0.1f);

			// Log actual distance to confirm stopping point
			float DistanceToEnemy = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
			//UE_LOG(LogTemp, Log, TEXT("Unit %s: Current distance to %s: %f"), *GetName(), *Enemy->GetName(), DistanceToEnemy);
		}
	}
	else
	{
		CurrentState = EUnitState::Idle;
		bFollowingOrders = false;
		CurrentTarget = nullptr;
		if (ACustomAIController* AIController = Cast<ACustomAIController>(GetController()))
		{
			AIController->StopMovement();
		}
		UE_LOG(LogTemp, Warning, TEXT("Unit %s: Pursuit failed - Enemy invalid"), *GetName());
	}
}

void AUnitClass::StartAttackMove(FVector TargetLocation)
{
    if (bCanMove)
    {
        AttackMoveTarget = TargetLocation;
        bAttackMove = true;
        CurrentState = EUnitState::Moving;
        bFollowingOrders = true;
        UE_LOG(LogTemp, Log, TEXT("Unit %s: Starting attack move to %s"), *GetName(), *TargetLocation.ToString());

        if (ACustomAIController* AIController = Cast<ACustomAIController>(GetController()))
        {
            AIController->MoveToLocation(TargetLocation, 20.0f); // Use default acceptance radius
        }
    }
}

void AUnitClass::UpdateAttackMove()
{
    if (bAttackMove)
    {
        UE_LOG(LogTemp, Log, TEXT("Unit %s: Updating attack move, checking for enemies"), *GetName());
        // Check for nearby enemies
        AActor* ClosestEnemy = nullptr;
        float ClosestDistance = AggroRange; // 500.0f for AScoutUnit
        for (TActorIterator<AUnitClass> It(GetWorld()); It; ++It)
        {
            AUnitClass* OtherUnit = *It;
            if (OtherUnit && OtherUnit != this && OtherUnit->TeamIDU != TeamIDU)
            {
                float Distance = FVector::Dist(GetActorLocation(), OtherUnit->GetActorLocation());
                if (Distance < ClosestDistance)
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
            float CapsuleOffset = 200.0f;
            float EffectiveAttackRange = AttackRange + 10.0f + CapsuleOffset; // 710.0f
            if (ClosestDistance <= EffectiveAttackRange)
            {
                CurrentState = EUnitState::Attacking;
                AttackAction(CurrentTarget);
                UE_LOG(LogTemp, Log, TEXT("Unit %s: Attacking detected enemy %s, Distance: %f"), *GetName(), *ClosestEnemy->GetName(), ClosestDistance);
            }
            else
            {
                CurrentState = EUnitState::Pursuing;
                PursueEnemy(CurrentTarget);
                UE_LOG(LogTemp, Log, TEXT("Unit %s: Pursuing detected enemy %s, Distance: %f"), *GetName(), *ClosestEnemy->GetName(), ClosestDistance);
            }
        }
        // No else clause; continue moving if no enemy is found
    }
}
