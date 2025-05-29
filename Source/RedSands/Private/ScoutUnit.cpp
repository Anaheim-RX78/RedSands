// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoutUnit.h"

#include "GameFramework/CharacterMovementComponent.h"


void AScoutUnit::OnSelected_Implementation(bool bIsSelected)
{
	Super::OnSelected_Implementation(bIsSelected);
}

void AScoutUnit::OnDamaged_Implementation(float DamageAmount)
{
	Super::OnDamaged_Implementation(DamageAmount);
}

void AScoutUnit::BeginPlay()
{
	Super::BeginPlay();
	Health = 100.f;
	Battery = 1000.f;
	Speed = 1000.f;
	AttackRange = 500.f;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}


