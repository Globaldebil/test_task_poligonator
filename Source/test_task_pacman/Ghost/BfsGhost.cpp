// Fill out your copyright notice in the Description page of Project Settings.


#include "BfsGhost.h"
#include "../Map/MazeGraph.h"
#include "Containers/Queue.h"


// Sets default values
ABfsGhost::ABfsGhost()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyColor = FLinearColor(FColor(255, 184, 255));
}

// Called when the game starts or when spawned
void ABfsGhost::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABfsGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<int32> ABfsGhost::FindPath(const FMazeGraph& Graph, int32 From, int32 To) const
{
	TMap<int32, int32> Parents;
	Parents.Add(From, From);

	TQueue<int32> Queue;
	Queue.Enqueue(From);

	while (!Queue.IsEmpty())
	{
		int32 Current;
		Queue.Dequeue(Current);

		if (Current == To)
		{
			break;
		}

		for (const FGraphEdge& Edge : Graph.GetNode(Current).Edges)
		{
			if (!Parents.Contains(Edge.ToNode))
			{
				Parents.Add(Edge.ToNode, Current);
				Queue.Enqueue(Edge.ToNode);
			}
		}
	}

	return BuildPath(Parents, From, To);
}
