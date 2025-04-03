// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm =CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerPawn::SetMovementInput(const FVector2D& MovementInput)
{
	//ATTUALE ROTAZIONE PAWN (PER MATCHARE IL MOVIMENTO ALLA TELECAMERA)
	FRotator PawnRotation = GetActorRotation();
	FVector ForwardDirection = FRotationMatrix(FRotator(0, PawnRotation.Yaw, 0)).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(FRotator(0, PawnRotation.Yaw, 0)).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDirection, MovementInput.X);
	AddMovementInput(RightDirection, MovementInput.Y);
}

void APlayerPawn::SetRotateInput(const float& RotationInput)
{
	//MODIFICA DELLA YAW DEL PAWN PER RUOTARE LA TELECAMERA DEL GIOCATORE
	FRotator PawnRotation = GetActorRotation();
	float NewYaw = PawnRotation.Yaw + RotationInput; 
	PawnRotation.Yaw = NewYaw;
	
	//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Green, FString::Printf(TEXT("Pawn Rotation: Pitch=%.2f, Yaw=%.2f,  Roll=%.2f"), PawnRotation.Pitch, PawnRotation.Yaw, PawnRotation.Roll));
	
	SetActorRotation(PawnRotation);
}

void APlayerPawn::SetZoomInput(const float& ZoomInput)
{
	//MODIFICA DELLA LUNGHEZZA DELLO SPRING ARM PER SIMULARE ZOOM DELLA TELECAMERA
	float NewZoom = ZoomInput * 50.f;
	NewZoom += SpringArm->TargetArmLength;
	
	//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("Camera Zoom: %.2f"), NewZoom));
	
	//TODO
	//ASSEGNARE VALORE MASSIMO E MINIMO DELLO ZOOM
	SpringArm->TargetArmLength = NewZoom;
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

