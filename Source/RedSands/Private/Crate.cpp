#include "Crate.h"
#include "Components/StaticMeshComponent.h"
#include "RTSPlayerstate.h"
#include "UnitClass.h"
#include "Kismet/GameplayStatics.h"

ACrate::ACrate()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ACrate::OnOverlapBegin);
}


void ACrate::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA<AUnitClass>())
	{
		CollectCrate(OtherActor);
	}
}

void ACrate::CollectCrate(AActor* Collector)
{
	if (AUnitClass* Unit = Cast<AUnitClass>(Collector))
	{
		if (APlayerController* PC = Unit->GetWorld()->GetFirstPlayerController())
		{
			if (ARTSPlayerstate* PlayerState = PC->GetPlayerState<ARTSPlayerstate>())
			{
				PlayerState->GainMinerals(MineralValue);
				Destroy();
			}
		}
	}
}

