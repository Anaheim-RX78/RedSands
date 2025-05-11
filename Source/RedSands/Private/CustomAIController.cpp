// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAIController.h"
#include "Navigation/CrowdFollowingComponent.h"

ACustomAIController::ACustomAIController()
{
		SetPathFollowingComponent(CreateDefaultSubobject<UCrowdFollowingComponent>(TEXT("CrowdFollowingComponent")));
}

void ACustomAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	
}
