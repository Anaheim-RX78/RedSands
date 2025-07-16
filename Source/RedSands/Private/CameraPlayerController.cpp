// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayerController.h"

#include "AIController.h"
#include "CustomAIController.h"
#include "MCVUnit.h"
#include "SelectInterface.h"
#include "UnitClass.h"
#include "Kismet/GameplayStatics.h"

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
	EnhancedInputComponent->BindAction(InputMap->Actions["Attack Move"], ETriggerEvent::Started, this, &ACameraPlayerController::AttackMove);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Started, this, &ACameraPlayerController::MultipleSelectStart);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Triggered, this, &ACameraPlayerController::MultipleSelectOnGoing);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Unit"], ETriggerEvent::Completed, this, &ACameraPlayerController::MultipleSelectEnd);
	EnhancedInputComponent->BindAction(InputMap->Actions["Ability Unit"], ETriggerEvent::Started, this, &ACameraPlayerController::UnitAbility);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select MCV"], ETriggerEvent::Started, this, &ACameraPlayerController::SelectMCV);
	EnhancedInputComponent->BindAction(InputMap->Actions["Select Combat Units"], ETriggerEvent::Started, this, &ACameraPlayerController::SelectCombatUnits);
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

	for (TActorIterator<AMCVUnit> It(GetWorld()); It; ++It)
	{
		AMCVUnit* MCV = *It;
		if (MCV && MCV->TeamIDU == PlayerPawn->TeamIDP)
		{
			PlayerMCV = MCV;
			break;
		}
	}
	if (BuildMenuWidgetClass)
	{
		BuildMenuWidget = CreateWidget<UUBuildMenuWidget>(this, BuildMenuWidgetClass);
	}
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

			if (AMCVUnit* HitMCV = Cast<AMCVUnit>(HitActor))
			{
				ShowBuildMenu(HitMCV);
			}
			else if (BuildMenuWidget && BuildMenuWidget->IsInViewport())
			{
				BuildMenuWidget->RemoveFromViewport();
			}
		}
		else if (SelectedActor)
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			SelectedActor = nullptr;

			if (BuildMenuWidget && BuildMenuWidget->IsInViewport())
			{
				BuildMenuWidget->RemoveFromViewport();
			}
		}
	}
	else
	{
		if (SelectedActor)
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			SelectedActor = nullptr;

			if (BuildMenuWidget && BuildMenuWidget->IsInViewport())
			{
				BuildMenuWidget->RemoveFromViewport();
			}
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

void ACameraPlayerController::SelectMCV(const FInputActionValue& Value)
{
	if (PlayerMCV && IsValid(PlayerMCV))
	{
		// Clear current selection
		if (SelectedActor)
		{
			if (SelectedActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
			{
				ISelectInterface::Execute_OnSelected(SelectedActor, false);
				UE_LOG(LogTemp, Log, TEXT("SelectMCV: Deselected single actor: %s"), *SelectedActor->GetName());
			}
			SelectedActor = nullptr;
		}
		for (AActor* Actor : SelectedActors)
		{
			if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
			{
				ISelectInterface::Execute_OnSelected(Actor, false);
				UE_LOG(LogTemp, Log, TEXT("SelectMCV: Deselected multi-selected actor: %s"), *Actor->GetName());
			}
		}
		SelectedActors.Empty();

		// Select the MCV
		SelectedActor = PlayerMCV;
		ISelectInterface::Execute_OnSelected(PlayerMCV, true);
		UE_LOG(LogTemp, Log, TEXT("SelectMCV: Selected MCV: %s"), *PlayerMCV->GetName());

		// Show build menu
		if (AMCVUnit* MCV = Cast<AMCVUnit>(PlayerMCV))
		{
			ShowBuildMenu(MCV);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectMCV: PlayerMCV is invalid"));
	}
}
void ACameraPlayerController::SelectCombatUnits(const FInputActionValue& Value)
{
	
	// Deselect current selection to avoid overlap
	if (SelectedActor)
	{
		ISelectInterface::Execute_OnSelected(SelectedActor, false);
		SelectedActor = nullptr;
	}
	for (AActor* Actor : SelectedActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Actor, false);
		}
	}
	SelectedActors.Empty();

	// Iterate through all units to find combat units
	TArray<AActor*> CombatUnits;
	for (TActorIterator<AUnitClass> It(GetWorld()); It; ++It)
	{
		AUnitClass* Unit = *It;
		if (Unit && Unit->bCanAttack && Unit->TeamIDU == PlayerPawn->TeamIDP)
		{
			CombatUnits.Add(Unit);
		}
	}

	// Select all found combat units
	for (AActor* Unit : CombatUnits)
	{
		if (Unit && Unit->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Unit, true);
			SelectedActors.Add(Unit);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Selected all combat units: %d"), SelectedActors.Num());
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
                // Only clear CurrentTarget if issuing a new attack order
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
                                AttackOrder(Unit, HitActor);
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

void ACameraPlayerController::AttackMove(const FInputActionValue& Value)
{
	if (SelectedActor || !SelectedActors.IsEmpty())
	{
		FHitResult Hit;
		if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Hit))
		{
			FVector TargetLocation = Hit.ImpactPoint;
			DrawDebugSphere(GetWorld(), TargetLocation, 25.0f, 12, FColor::Purple, false, 2.0f); // Purple for attack move

			if (SelectedActor)
			{
				if (AUnitClass* Unit = Cast<AUnitClass>(SelectedActor))
				{
					if (Unit->bCanMove && Unit->TeamIDU == PlayerPawn->TeamIDP)
					{
						Unit->StartAttackMove(TargetLocation);
					}
				}
			}
			else if (!SelectedActors.IsEmpty())
			{
				for (AActor* Actor : SelectedActors)
				{
					if (AUnitClass* Unit = Cast<AUnitClass>(Actor))
					{
						if (Unit->bCanMove && Unit->TeamIDU == PlayerPawn->TeamIDP)
						{
							Unit->StartAttackMove(TargetLocation);
						}
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No unit selected, attack move ignored"));
	}
}

void ACameraPlayerController::ShowBuildMenu(AMCVUnit* SelectedMCV)
{
	if (BuildMenuWidget && SelectedMCV)
	{
		BuildMenuWidget->SetSelectedMCV(SelectedMCV);
		if (!BuildMenuWidget->IsInViewport())
		{
			BuildMenuWidget->AddToViewport();
		}
	}
	else if (BuildMenuWidget && BuildMenuWidget->IsInViewport())
	{
		BuildMenuWidget->RemoveFromViewport();
	}
}

void ACameraPlayerController::UnitAbility(const FInputActionValue& Value)
{
	if (SelectedActor)
	{
		if (AUnitClass* Unit = Cast<AUnitClass>(SelectedActor))
		{
			Unit->Ability();
		}
	}
	else if (!SelectedActors.IsEmpty())
	{
		for (AActor* Actor : SelectedActors)
		{
			if (AUnitClass* Unit = Cast<AUnitClass>(Actor))
			{
				Unit->Ability();
			}
		}
	}
}
void ACameraPlayerController::AttackOrder(AActor* UnitActor, AActor* TargetActor)
{
	if (AUnitClass* Unit = Cast<AUnitClass>(UnitActor))
	{
		if (TargetActor && IsValid(TargetActor))
		{
			Unit->CurrentTarget = TargetActor;
			Unit->bFollowingOrders = true;
			float CapsuleOffset = 200.0f; // Match AttackAction
			float EffectiveAttackRange = Unit->AttackRange + 10.0f + CapsuleOffset; // 710.0f
			float DistanceToTarget = FVector::Dist(Unit->GetActorLocation(), TargetActor->GetActorLocation());
			if (DistanceToTarget <= EffectiveAttackRange)
			{
				Unit->CurrentState = EUnitState::Attacking;
			}
			else
			{
				Unit->CurrentState = EUnitState::Pursuing;
				Unit->PursueEnemy(TargetActor);
			}
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unit %s: Attack order issued for %s, Distance: %f"), *Unit->GetName(), *TargetActor->GetName(), DistanceToTarget));
		}
	}
}

void ACameraPlayerController::MultiSelectActors(TArray<AActor*> BoxSelectActors)
{
	// Deselect current SelectedActor (e.g., MCV from SelectMCV)
	if (SelectedActor)
	{
		if (SelectedActor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(SelectedActor, false);
			UE_LOG(LogTemp, Log, TEXT("MultiSelectActors: Deselected single actor: %s"), *SelectedActor->GetName());
		}
		SelectedActor = nullptr;
	}

	// Deselect current multi-selected actors
	for (AActor* Actor : SelectedActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Actor, false);
			UE_LOG(LogTemp, Log, TEXT("MultiSelectActors: Deselected actor: %s"), *Actor->GetName());
		}
	}
	SelectedActors.Empty();

	// Select new actors
	bool bHasMCV = false;
	for (AActor* Actor : BoxSelectActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(USelectInterface::StaticClass()))
		{
			ISelectInterface::Execute_OnSelected(Actor, true);
			SelectedActors.Add(Actor);
			UE_LOG(LogTemp, Log, TEXT("MultiSelectActors: Selected actor: %s"), *Actor->GetName());
			if (Actor == PlayerMCV)
			{
				bHasMCV = true;
			}
		}
	}

	// Manage build menu
	if (bHasMCV)
	{
		if (AMCVUnit* MCV = Cast<AMCVUnit>(PlayerMCV))
		{
			ShowBuildMenu(MCV);
			UE_LOG(LogTemp, Log, TEXT("MultiSelectActors: Showed build menu for MCV"));
		}
	}
	else if (BuildMenuWidget && BuildMenuWidget->IsInViewport())
	{
		BuildMenuWidget->RemoveFromViewport();
		UE_LOG(LogTemp, Log, TEXT("MultiSelectActors: Hid build menu (no MCV in selection)"));
	}
}

void ACameraPlayerController::GameOver(bool winorlose)
{
	if (!winorlose)
	{
		if (GameOverWidgetClass)
		{
			GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
			if (GameOverWidget)
			{
				GameOverWidget->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeUIOnly());
				UGameplayStatics::SetGamePaused(GetWorld(), true);
				UE_LOG(LogTemp, Log, TEXT("CameraPlayerController: Game over screen shown"));
			}
		}
		
	
	}
	else if (winorlose)
	{
		if (VictoryWidgetClass)
		{
			VictoryWidget = CreateWidget<UUserWidget>(this, VictoryWidgetClass);
			if (VictoryWidget)
			{
				VictoryWidget->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeUIOnly());
				UGameplayStatics::SetGamePaused(GetWorld(), true);
				UE_LOG(LogTemp, Log, TEXT("CameraPlayerController: Victory screen shown"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CameraPlayerController: VictoryWidgetClass not assigned"));
		}
	}
}


