// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerPill.h"
#include "../Game/PacmanGameMode.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APowerPill::APowerPill()
{
	Points = 50;

	Collision->SetSphereRadius(30.f);
	Mesh->SetRelativeScale3D(FVector(0.6f));
}

void APowerPill::OnEaten(APacmanGameMode* GameMode)
{
	GameMode->FrightenGhosts();
	Super::OnEaten(GameMode);
}
