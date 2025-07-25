#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SelectInterface.h"
#include "DamageInterface.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "UnitClass.generated.h"


UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Idle,
	Moving,
	Attacking,
	Pursuing
};

UCLASS()
class REDSANDS_API AUnitClass : public ACharacter, public ISelectInterface,public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitClass();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EUnitState CurrentState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Battery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFollowingOrders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TeamIDU;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AggroRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CurrentTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SelectionBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PlaceholderMesh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	FTimerHandle HealthBarHideTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* MuzzleFlashComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UNiagaraSystem* MuzzleFlashSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AttackMoveTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAttackMove;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	FTimerHandle AttackTimerHandle;
	
	UPROPERTY()
	float LastAttackTime = -1000.0f;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnSelected_Implementation(bool bIsSelected)override;
	UFUNCTION(BlueprintCallable)
	virtual void OnDamaged_Implementation(float DamageAmount) override;
	
	UFUNCTION()
	virtual void MovementAction(FVector Location);

	UFUNCTION()
	virtual void ProximityAggro();
	
	UFUNCTION()
	virtual void AttackAction(AActor* Enemy);

	UFUNCTION()
	virtual void Ability();
	
	UFUNCTION()
	virtual void PursueEnemy(AActor* Enemy);

	UFUNCTION()
	void StartAttackMove(FVector TargetLocation);

	UFUNCTION()
	void UpdateAttackMove();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

