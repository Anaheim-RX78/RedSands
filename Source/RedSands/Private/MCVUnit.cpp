// Fill out your copyright notice in the Description page of Project Settings.


#include "MCVUnit.h"
#include "CustomAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

void AMCVUnit::OnSelected_Implementation(bool bIsSelected)
{
	Super::OnSelected_Implementation(bIsSelected);
}

void AMCVUnit::OnDamaged_Implementation(float DamageAmount)
{
	Super::OnDamaged_Implementation(DamageAmount);
}

void AMCVUnit::BeginPlay()
{
	Super::BeginPlay();
	Health = 1000.f;
	Battery = 1000.f;
	Speed = 500.f;
	AttackRange = 0.f;
	AttackDamage = 0.f;
	AttackInterval = 0.f;
	bCanAttack = false;
	bCanMove = true;
	bBuildMode = false;
	CurrentState = EUnitState::Idle;
	bFollowingOrders = false;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AMCVUnit::Ability()
{
	Super::Ability();
	if (bBuildMode)
	{
		bCanMove=true;
		bBuildMode=false;
	}
	else
	{
		bCanMove=false;
		bBuildMode=true;
	}
}
