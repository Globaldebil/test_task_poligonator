// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ghost.h"
#include "DfsGhost.generated.h"

UCLASS()
class TEST_TASK_PACMAN_API ADfsGhost : public AGhost
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADfsGhost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual TArray<int32> FindPath(const FMazeGraph& Graph, int32 From, int32 To) const override;
	virtual bool FollowsWholePath() const override { return true; }

private:
	bool DepthFirstSearch(const FMazeGraph& Graph, int32 Current, int32 To, TSet<int32>& Visited, TArray<int32>& Path) const;
	int32 GetDistance(FIntPoint From, FIntPoint To) const;
	
};
