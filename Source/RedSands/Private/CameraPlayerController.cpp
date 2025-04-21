// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayerController.h"

#include "SelectInterface.h"
#include "UnitClass.h"

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
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Triggered, this, &ACameraPlayerController::Select);
	EnhancedInputComponent->BindAction(InputMap->Actions["Action Unit"], ETriggerEvent::Triggered, this, &ACameraPlayerController::UnitAction);
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

void ACameraPlayerController::Select(const FInputActionValue& Value)
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, HitResult))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			if (SelectedActor && SelectedActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
			{
				ISelectInterface::Execute_OnSelected(SelectedActor, false);
			}

			SelectedActor = HitActor;
			ISelectInterface::Execute_OnSelected(SelectedActor, true);
		}
		else if (SelectedActor != nullptr)
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			SelectedActor = nullptr;  // Deselect if not valid
		}
	}
	else
	{
		if (SelectedActor != nullptr)  // Only deselect if there was a selection
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			SelectedActor = nullptr;
		}
	}
}

void ACameraPlayerController::UnitAction(const FInputActionValue& Value)
{
	if (SelectedActor) 
	{
		if (AUnitClass* Unit = Cast<AUnitClass>(SelectedActor))
		{
			if (Unit->bCanMove)
			{
				FHitResult Hit;
				
				if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Hit))
				{
					FVector TargetLocation = Hit.ImpactPoint;
					
					DrawDebugSphere(GetWorld(), TargetLocation, 25.0f, 12, FColor::Green, false, 2.0f);
					
					Unit->MovementAction(TargetLocation);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No unit selected, move command ignored"));
	}
}