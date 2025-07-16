#include "Objective.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "CameraPlayerController.h"

AObjective::AObjective()
{
    PrimaryActorTick.bCanEverTick = true;
    
    Health = 1000.0f;
    CurrentHealth = Health;
    TeamIDU = 1; // Enemy team
    bCanMove = false; // Static objective
    bCanAttack = false; // No attacking capability
}

void AObjective::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("Objective %s: Initialized with Health = %f, TeamIDU = %d"), *GetName(), CurrentHealth, TeamIDU);
}

void AObjective::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AObjective::OnSelected_Implementation(bool bIsSelected)
{
    ISelectInterface::OnSelected_Implementation(bIsSelected);
    SelectionBase->SetVisibility(bIsSelected);
    UE_LOG(LogTemp, Log, TEXT("Objective %s: Selected = %d"), *GetName(), bIsSelected);
}

void AObjective::OnDamaged_Implementation(float DamageAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, Health);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s Health: %f, MaxHealth: %f"), *GetName(), CurrentHealth, Health));
    float HealthPercent = Health > 0.0f ? CurrentHealth / Health : 0.0f;
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Percent: %f"), HealthPercent));

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
    }

    if (CurrentHealth <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Objective %s: Destroyed"), *GetName()));
        GameOver();
    }
}

void AObjective::GameOver()
{
    if (ACameraPlayerController* PlayerController = Cast<ACameraPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PlayerController->GameOver(true);
        UE_LOG(LogTemp, Log, TEXT("Objective %s: Triggering victory screen"), *GetName());
    }
}