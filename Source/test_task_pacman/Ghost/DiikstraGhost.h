// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ghost.h"
#include "DiikstraGhost.generated.h"

struct FDijkstraQueueItem
{
	int32 Node = INDEX_NONE;
	int32 Distance = 0;

	FDijkstraQueueItem() {}
	FDijkstraQueueItem(int32 InNode, int32 InDistance) : Node(InNode), Distance(InDistance) {}

	bool operator<(const FDijkstraQueueItem& Other) const
	{
		return Distance < Other.Distance;
	}
};

UCLASS()
class TEST_TASK_PACMAN_API ADiikstraGhost : public AGhost
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADiikstraGhost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual TArray<int32> FindPath(const FMazeGraph& Graph, int32 From, int32 To) const override;
};
