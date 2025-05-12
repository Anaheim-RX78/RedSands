#include "UnitClass.h"
#include "AIController.h"
#include "CustomAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	Health-= DamageAmount;
}


void AUnitClass::MovementAction(FVector Location)
{
	if (bCanMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to move to: %s"), *Location.ToString());

		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIController found: %s"), *AIController->GetName());
			AIController->MoveToLocation(Location, 10.0f);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No AIController found for: %s"), *GetName());
		}
	}
}


