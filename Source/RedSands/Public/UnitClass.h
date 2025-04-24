#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SelectInterface.h"
#include "UnitClass.generated.h"

UCLASS()
class REDSANDS_API AUnitClass : public ACharacter, public ISelectInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Battery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TeamIDU;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SelectionBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PlaceholderMesh;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnSelected_Implementation(bool bIsSelected) override;

	virtual void MovementAction(FVector Location);
};

