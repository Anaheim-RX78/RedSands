#include "UnitClass.h"
#include "AIController.h"
#include "CustomAIController.h"
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
		if (GetCharacterMovement()->Velocity.IsNearlyZero() && !bFollowingOrders)
		{
			CurrentState = EUnitState::Idle;
		}
	}
	else if (CurrentState == EUnitState::Idle && !bFollowingOrders)
	{
		ProximityAggro();
	}
	else if (CurrentState == EUnitState::Attacking)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("DID WE GET HERE OR NOT?")));
		AttackAction(CurrentTarget);
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
		UE_LOG(LogTemp, Warning, TEXT("Trying to move to: %s"), *Location.ToString());
		CurrentTarget = nullptr;
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
/*	TArray<AActor*> OverlappingActors;

	AActor* ClosestEnemy = nullptr;
	float ClosestDistance = AggroRange;
	
	UKismetSystemLibrary::SphereOverlapActors(
	   GetWorld(),
	   GetActorLocation(),
	   AggroRange,
	   TArray<TEnumAsByte<EObjectTypeQuery>>{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_EngineTraceChannel1) },
	   nullptr,
	   TArray<AActor*>{ this },
	   OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AUnitClass* Enemy = Cast<AUnitClass>(Actor);
		if (TeamIDU != Enemy->TeamIDU)
		{
			float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestEnemy = Actor;
				ClosestDistance = Distance;
			}
		}
	}

	if (ClosestEnemy)
	{
		CurrentTarget = ClosestEnemy;
		if (ClosestDistance <= AttackRange)
		{
			AttackAction(CurrentTarget);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,TEXT("PURSUEING"));
			PursueEnemy(CurrentTarget);
		}
	}
	else
	{
		CurrentTarget = nullptr;
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
		}
	}*/
	
}
void AUnitClass::AttackAction(AActor* Enemy)
{

}

void AUnitClass::PursueEnemy(AActor* Enemy)
{
	if (ACustomAIController* AIController = Cast<ACustomAIController>(GetController()))
	{
		CurrentState = EUnitState::Moving;
		AIController->MoveToActor(Enemy, AttackRange - 10.0f);
	}
}