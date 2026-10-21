// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PowerPill.generated.h"

UCLASS()
class TEST_TASK_PACMAN_API APowerPill : public APickup
{
	GENERATED_BODY()

public:
	APowerPill();

	virtual void OnEaten(APacmanGameMode* GameMode) override;
};
