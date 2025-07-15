// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Gameplay.h"

#include "CameraPlayerController.h"
#include "CanvasTypes.h"
#include "EngineUtils.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

void AGM_Gameplay::BeginPlay()
{
	Super::BeginPlay();
	FogRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	FogRenderTarget->InitAutoFormat(256, 256); // Adjust size as needed
	FogRenderTarget->ClearColor = FLinearColor::Black;
	FogRenderTarget->UpdateResource();
	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ACameraPlayerController* CameraPC = Cast<ACameraPlayerController>(PC))
		{
			if (CameraPC->PlayerPawn)
			{
				PlayerTeamID = CameraPC->PlayerPawn->TeamIDP;
				UE_LOG(LogTemp, Log, TEXT("Game Mode initialized with Player Team ID: %d"), PlayerTeamID);
			}
		}
	}
	
}

void AGM_Gameplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGM_Gameplay::UpdateFogOfWar()
{
 
}

