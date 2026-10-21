// Fill out your copyright notice in the Description page of Project Settings.


#include "DiikstraGhost.h"
#include "../Map/MazeGraph.h"


// Sets default values
ADiikstraGhost::ADiikstraGhost()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyColor = FLinearColor::Red;
}

// Called when the game starts or when spawned
void ADiikstraGhost::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADiikstraGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<int32> ADiikstraGhost::FindPath(const FMazeGraph& Graph, int32 From, int32 To) const
{
	TArray<int32> Distances;
	Distances.Init(MAX_int32, Graph.GetNodeCount());
	Distances[From] = 0;

	TMap<int32, int32> Parents;
	Parents.Add(From, From);

	TArray<FDijkstraQueueItem> Queue;
	Queue.HeapPush(FDijkstraQueueItem(From, 0));

	while (Queue.Num() > 0)
	{
		FDijkstraQueueItem Item;
		Queue.HeapPop(Item);

		if (Item.Node == To)
		{
			break;
		}
		if (Item.Distance > Distances[Item.Node])
		{
			continue;
		}

		for (const FGraphEdge& Edge : Graph.GetNode(Item.Node).Edges)
		{
			int32 NewDistance = Item.Distance + Edge.Weight;
			if (NewDistance < Distances[Edge.ToNode])
			{
				Distances[Edge.ToNode] = NewDistance;
				Parents.Add(Edge.ToNode, Item.Node);
				Queue.HeapPush(FDijkstraQueueItem(Edge.ToNode, NewDistance));
			}
		}
	}

	return BuildPath(Parents, From, To);
}
