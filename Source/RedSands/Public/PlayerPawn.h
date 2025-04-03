// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerPawn.generated.h"

UCLASS()
class REDSANDS_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//RICEVE INPUT DAL PLAYER CONTROLLER PER GESTIRE IL MOVIMENTO DELLA CAMERA TRAMITE (WASD)
	void SetMovementInput(const FVector2D& MovementInput);
	//RICEVE INPUT DAL PLAYER CONTROLLER PER GESTIRE LA "ROTAZIONE" DELLE TELECAMERA CON (Q) E (E)
	void SetRotateInput(const float& RotationInput);
	//RICEVE INPUT DAL PLAYER CONTROLLER PER GESTIRE LO ZOOM DELLA TELECAMERA CON (MOUSE SCROLL)
	void SetZoomInput(const float& ZoomInput);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
