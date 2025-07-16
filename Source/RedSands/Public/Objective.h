#pragma once
#include "CoreMinimal.h"
#include "UnitClass.h"
#include "Components/WidgetComponent.h"
#include "Objective.generated.h"

UCLASS()
class REDSANDS_API AObjective : public AUnitClass
{
	GENERATED_BODY()
public:
	AObjective();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnSelected_Implementation(bool bIsSelected) override;
	
	virtual void OnDamaged_Implementation(float DamageAmount) override;

	UFUNCTION(BlueprintCallable)
	void GameOver();

private:
	FTimerHandle HealthBarHideTimer;
};