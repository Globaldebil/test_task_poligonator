// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ghost.h"
#include "BellmanFordGhost.generated.h"

struct FBellmanFordEdge
{
	int32 From = 0;
	int32 To = 0;
	int32 Weight = 0;

	FBellmanFordEdge() {}
	FBellmanFordEdge(int32 InFrom, int32 InTo, int32 InWeight) : From(InFrom), To(InTo), Weight(InWeight) {}
};

UCLASS()
class TEST_TASK_PACMAN_API ABellmanFordGhost : public AGhost
{
	GENERATED_BODY()

public:
	ABellmanFordGhost();

	virtual TArray<int32> FindPath(const FMazeGraph& Graph, int32 From, int32 To) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Ghost|BellmanFord")
	int32 FoodBonus = 2;

private:
	TArray<int32> GetDepths(const FMazeGraph& Graph, int32 From) const;
	TArray<FBellmanFordEdge> GetForwardEdges(const FMazeGraph& Graph, int32 From) const;
	bool HasNegativeCycle(const TArray<FBellmanFordEdge>& Edges, const TArray<int32>& Distances) const;
};
