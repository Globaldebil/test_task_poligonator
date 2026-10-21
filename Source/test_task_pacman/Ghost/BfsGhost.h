// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ghost.h"
#include "BfsGhost.generated.h"

UCLASS()
class TEST_TASK_PACMAN_API ABfsGhost : public AGhost
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABfsGhost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual TArray<int32> FindPath(const FMazeGraph& Graph, int32 From, int32 To) const override;
};
