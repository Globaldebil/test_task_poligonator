// Fill out your copyright notice in the Description page of Project Settings.


#include "DfsGhost.h"
#include "../Map/MazeGraph.h"



// Sets default values
ADfsGhost::ADfsGhost()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyColor = FLinearColor(FColor(255, 184, 82));
}

// Called when the game starts or when spawned
void ADfsGhost::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADfsGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<int32> ADfsGhost::FindPath(const FMazeGraph& Graph, int32 From, int32 To) const
{
	TSet<int32> Visited;
	TArray<int32> Path;
	DepthFirstSearch(Graph, From, To, Visited, Path);
	return Path;
}

bool ADfsGhost::DepthFirstSearch(const FMazeGraph& Graph, int32 Current, int32 To, TSet<int32>& Visited, TArray<int32>& Path) const
{
	Visited.Add(Current);
	Path.Add(Current);

	if (Current == To)
	{
		return true;
	}

	FIntPoint Target = Graph.GetNode(To).Tile;
	TArray<FGraphEdge> Edges = Graph.GetNode(Current).Edges;
	Edges.StableSort([this, &Graph, Target](const FGraphEdge& A, const FGraphEdge& B)
	{
		return GetDistance(Graph.GetNode(A.ToNode).Tile, Target) < GetDistance(Graph.GetNode(B.ToNode).Tile, Target);
	});

	for (const FGraphEdge& Edge : Edges)
	{
		if (!Visited.Contains(Edge.ToNode) && DepthFirstSearch(Graph, Edge.ToNode, To, Visited, Path))
		{
			return true;
		}
	}

	Path.RemoveAt(Path.Num() - 1);
	return false;
}

int32 ADfsGhost::GetDistance(FIntPoint From, FIntPoint To) const
{
	return FMath::Abs(From.X - To.X) + FMath::Abs(From.Y - To.Y);
}
