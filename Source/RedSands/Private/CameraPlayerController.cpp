// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayerController.h"

#include "AIController.h"
#include "CustomAIController.h"
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
	//EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Completed, this, &ACameraPlayerController::Select);
	EnhancedInputComponent->BindAction(InputMap->Actions["Action Unit"], ETriggerEvent::Started, this, &ACameraPlayerController::UnitAction);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Started, this, &ACameraPlayerController::MultipleSelectStart);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Triggered, this, &ACameraPlayerController::MultipleSelectOnGoing);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Completed, this, &ACameraPlayerController::MultipleSelectEnd);
}

void ACameraPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (APlayerPawn* PlayerPawnC = Cast<APlayerPawn>(InPawn)) {PlayerPawn = PlayerPawnC;}
}

void ACameraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	TopDownHud = Cast<AToDownHUD>(GetHUD());
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
	//Deselect ALL multi-selected actors
	for (AActor* Actor : SelectedActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Actor, false);
		}
	}
	SelectedActors.Empty();

	// Then handle single-click selection
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, HitResult))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			// Deselect previously selected single actor
			if (SelectedActor && SelectedActor != HitActor && SelectedActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
			{
				ISelectInterface::Execute_OnSelected(SelectedActor, false);
			}

			SelectedActor = HitActor;
			ISelectInterface::Execute_OnSelected(SelectedActor, true);
		}
		else if (SelectedActor)
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			SelectedActor = nullptr;
		}
	}
	else
	{
		if (SelectedActor)
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			SelectedActor = nullptr;
		}
	}
}


void ACameraPlayerController::MultipleSelectStart(const FInputActionValue& Value)
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	SelectionStartPosition = FVector2D(MouseX, MouseY);
	
}

void ACameraPlayerController::MultipleSelectOnGoing(const FInputActionValue& Value)
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	SelectionSize = FVector2D(MouseX-SelectionStartPosition.X, MouseY-SelectionStartPosition.Y);
	if (TopDownHud)
	{
		TopDownHud->ShowRectangleBox(SelectionStartPosition, SelectionSize);
	}
}

void ACameraPlayerController::MultipleSelectEnd(const FInputActionValue& Value)
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	FVector2D CurrentPosition(MouseX, MouseY);

	if (TopDownHud)
	{
		TopDownHud->HideRectangleBox();
	}

	FVector2D SelectionDelta = CurrentPosition - SelectionStartPosition;
	const float DragThreshold = 5.0f; // pixels

	if (FMath::Abs(SelectionDelta.X) < DragThreshold && FMath::Abs(SelectionDelta.Y) < DragThreshold)
	{
		Select(Value);
		return;
	}
	
	FVector2D TopLeft(FMath::Min(SelectionStartPosition.X, MouseX), FMath::Min(SelectionStartPosition.Y, MouseY));
	FVector2D BottomRight(FMath::Max(SelectionStartPosition.X, MouseX), FMath::Max(SelectionStartPosition.Y, MouseY));
	FVector2D TopRight(BottomRight.X, TopLeft.Y);
	FVector2D BottomLeft(TopLeft.X, BottomRight.Y);

	TArray<FVector> NearPoints;
	TArray<FVector> FarPoints;
	const float TraceDistance = 100000.0f;
	TArray<FVector2D> ScreenPoints = { TopLeft, TopRight, BottomRight, BottomLeft };

	for (const FVector2D& ScreenPoint : ScreenPoints)
	{
		FVector WorldOrigin, WorldDirection;
		if (DeprojectScreenPositionToWorld(ScreenPoint.X, ScreenPoint.Y, WorldOrigin, WorldDirection))
		{
			NearPoints.Add(WorldOrigin);
			FarPoints.Add(WorldOrigin + WorldDirection * TraceDistance);
		}
	}

	FConvexVolume SelectionVolume;
	auto MakePlane = [](const FVector& A, const FVector& B, const FVector& C)
	{
		return FPlane(A, B, C);
	};

	SelectionVolume.Planes.Add(MakePlane(NearPoints[1], NearPoints[0], FarPoints[0]));
	SelectionVolume.Planes.Add(MakePlane(NearPoints[3], NearPoints[2], FarPoints[2]));
	SelectionVolume.Planes.Add(MakePlane(NearPoints[2], NearPoints[1], FarPoints[1]));
	SelectionVolume.Planes.Add(MakePlane(NearPoints[0], NearPoints[3], FarPoints[3]));
	SelectionVolume.Planes.Add(MakePlane(NearPoints[0], NearPoints[1], NearPoints[2]));
	SelectionVolume.Planes.Add(MakePlane(FarPoints[2], FarPoints[1], FarPoints[0]));
	SelectionVolume.Init();

	TArray<AActor*> NewlySelected;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		if (Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			const FBox Bounds = Actor->GetComponentsBoundingBox();
			if (SelectionVolume.IntersectBox(Bounds.GetCenter(), Bounds.GetExtent()))
			{
				NewlySelected.Add(Actor);
			}
		}
	}

	MultiSelectActors(NewlySelected);
}


void ACameraPlayerController::UnitAction(const FInputActionValue& Value)
{
	if (SelectedActor) 
	{
		if (AUnitClass* Unit = Cast<AUnitClass>(SelectedActor))
		{
			if (Unit->bCanMove && Unit->TeamIDU == PlayerPawn->TeamIDP)
			{
				FHitResult Hit;
				bool bDidAttack = false;
				Unit->CurrentTarget = nullptr;
				if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, Hit))
				{
					AActor* HitActor = Hit.GetActor();
					if (HitActor && HitActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
					{
						if (AUnitClass* TargetUnit = Cast<AUnitClass>(HitActor))
						{
							if (TargetUnit->TeamIDU != PlayerPawn->TeamIDP)
							{
								AttackOrder(Unit, TargetUnit);
								bDidAttack = true;
							}
						}
					}
				}
				
				if (!bDidAttack && GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Hit))
				{
					FVector TargetLocation = Hit.ImpactPoint;

					DrawDebugSphere(GetWorld(), TargetLocation, 25.0f, 12, FColor::Green, false, 2.0f);

					Unit->MovementAction(TargetLocation);
				}
			}
		}
	}

	else if (!SelectedActors.IsEmpty())
	{
		FHitResult Hit;
		
		if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, Hit))
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
			{
				AUnitClass* TargetUnit = Cast<AUnitClass>(HitActor);
				if (TargetUnit && TargetUnit->TeamIDU != PlayerPawn->TeamIDP)
				{
					for (AActor* Actor : SelectedActors)
					{
						if (AUnitClass* Unit = Cast<AUnitClass>(Actor))
						{
							if (Unit->bCanMove && Unit->TeamIDU == PlayerPawn->TeamIDP)
							{
								Unit->CurrentTarget = nullptr;
								AttackOrder(Unit,HitActor);
							}
						}
					}

					return;
				}
			}
		}
		
		if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Hit))
		{
			FVector TargetLocation = Hit.ImpactPoint;

			DrawDebugSphere(GetWorld(), TargetLocation, 25.0f, 12, FColor::Green, false, 2.0f);

			for (AActor* Actor : SelectedActors)
			{
				if (AUnitClass* Unit = Cast<AUnitClass>(Actor))
				{
					if (Unit->bCanMove && Unit->TeamIDU == PlayerPawn->TeamIDP)
					{
						Unit->MovementAction(TargetLocation);
					}
				}
			}
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No unit selected, move command ignored"));
	}
}

void ACameraPlayerController::AttackOrder(AActor* UnitActor, AActor* TargetActor)
{
	
	if (AUnitClass* Unit = Cast<AUnitClass>(UnitActor))
	{
		Unit->CurrentTarget = TargetActor;
		Unit->bFollowingOrders = true;
		Unit->CurrentState = EUnitState::Attacking;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BIG ORDER")));
		if (Unit->CurrentTarget)
		{
			AAIController* UnitController = Cast<ACustomAIController>(Unit->GetController());
			UnitController->MoveToActor(TargetActor,Unit->AttackRange);
		}
	} 
	
}

void ACameraPlayerController::MultiSelectActors(TArray<AActor*> BoxSelectActors)
{
	SelectedActor = nullptr; // Clear any previous single-click selection

	for (AActor* Actor : SelectedActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Actor, false);
		}
	}

	SelectedActors.Empty();

	for (AActor* Actor : BoxSelectActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Actor, true);
			SelectedActors.Add(Actor);
		}
	}
}
