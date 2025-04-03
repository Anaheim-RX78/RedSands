// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayerController.h"

void ACameraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	bShowMouseCursor = true;

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	Subsystem->ClearAllMappings();

	if (!InputMap)
	{
		GEngine->AddOnScreenDebugMessage(-1,2,FColor::Red,TEXT("CONTROLLA CHE CI SIA L'INPUT MAP"));
	}

	Subsystem->AddMappingContext(InputMap->Context,0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast to EnhancedInputComponent"));
	}

	EnhancedInputComponent->BindAction(InputMap->Actions["Move Camera"], ETriggerEvent::Triggered, this, &ACameraPlayerController::Move);
	EnhancedInputComponent->BindAction(InputMap->Actions["Rotate Camera"], ETriggerEvent::Triggered, this, &ACameraPlayerController::Rotate);
	EnhancedInputComponent->BindAction(InputMap->Actions["Zoom Camera"], ETriggerEvent::Triggered, this, &ACameraPlayerController::Zoom);
}

void ACameraPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (APlayerPawn* PlayerPawnC = Cast<APlayerPawn>(InPawn)) {PlayerPawn = PlayerPawnC;}
}

void ACameraPlayerController::Move(const FInputActionValue& Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->SetMovementInput(Value.Get<FVector2D>());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is not set or is null!"));
	}
}

void ACameraPlayerController::Rotate(const FInputActionValue& Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->SetRotateInput(Value.Get<float>());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is not set or is null!"));
	}
}

void ACameraPlayerController::Zoom(const FInputActionValue& Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->SetZoomInput(Value.Get<float>());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is not set or is null!"));
	}
}